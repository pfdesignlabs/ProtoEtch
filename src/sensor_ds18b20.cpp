#include <OneWire.h>
#include <DallasTemperature.h>
#include <Preferences.h>
#include "config.h"
#include "sensor_ds18b20.h"

namespace {
  // Hardware & Dallas
  OneWire           oneWire(TS_PIN);
  DallasTemperature ds(&oneWire);
  DeviceAddress     addr{};
  bool              hasDev = false;

  // State & timing
  enum State { IDLE, START_CONV, WAIT_CONV, READ };
  State     st       = IDLE;
  uint32_t  periodMs = TS_DEFAULT_PERIOD_MS;
  uint32_t  tNext    = 0;
  uint32_t  tConv    = 0;
  uint8_t   resBits  = TS_RES;
  const uint32_t MAX_TIMEOUT_MS = TS_TIMEOUT_MS;

  // Backoff bij fouten (period << backoffPow)
  uint8_t   backoffPow = 0; // 0..TS_MAX_BACKOFF_POW

  // Filtering
  float emaAlpha = 0.0f;    // 0 = smoothing uit
  float emaValue = NAN;

  // Kalibratie
  float kGain   = 1.0f;
  float kOffset = 0.0f;

  // Resultaat & callbacks
  TempSensor::Sample    last{};
  TempSensor::Callback  listeners[4] = {nullptr,nullptr,nullptr,nullptr};

  // Errors & stats
  TempSensor::Error gErr = TempSensor::OK;
  TempSensor::Stats gSt;

  void notify(const TempSensor::Sample& s) {
    for (auto &cb : listeners) if (cb) cb(s);
  }

  // Plan volgende meetmoment op basis van backoff
  void scheduleNext(uint32_t now, bool ok) {
    if (ok) backoffPow = 0;
    else if (backoffPow < TS_MAX_BACKOFF_POW) backoffPow++;

    const uint32_t delayMs = (ok ? periodMs : (periodMs << backoffPow));
    tNext = now + delayMs;
  }

  // Lees & valideer scratchpad + (optioneel) gebruik Dallas helper voor temp
  bool readWithCRC(float &outC) {
    uint8_t sp[9];
    if (!ds.readScratchPad(addr, sp)) {
      return false; // leesfout
    }
    uint8_t crc = OneWire::crc8(sp, 8);
    if (crc != sp[8]) {
      return false; // CRC mismatch
    }
    // We kunnen temp uit scratchpad berekenen, maar DallasTemperature::getTempC
    // gebruikt eveneens de scratchpad. We hebben de CRC nu al geverifieerd.
    float t = ds.getTempC(addr);
    outC = t;
    return true;
  }
} // namespace

namespace TempSensor {

  void begin() {
    ds.begin();
    if (ds.getAddress(addr, 0)) {
      hasDev = true;
      ds.setResolution(addr, resBits);
      ds.setWaitForConversion(false); // non-blocking conversies
      loadCalibration();

      last = {};
      tNext = millis();  // direct starten
      LOGI("[Temp] DS18B20 ROM=");
      for (uint8_t i=0;i<8;i++) LOGI("%02X", addr[i]);
      LOGI("  %u-bit | gain=%.4f off=%.4f\n", resBits, kGain, kOffset);
    } else {
      hasDev = false;
      gErr = NO_DEVICE;
      LOGW("[Temp] No DS18B20 detected on pin %d\n", TS_PIN);
    }
  }

  void setPeriod(uint32_t ms) {
    if (ms < 100) ms = 100;
    periodMs = ms;
  }

  void setResolution(uint8_t bits) {
    if (bits < 9) bits = 9; if (bits > 12) bits = 12;
    resBits = bits;
    if (hasDev) ds.setResolution(addr, resBits);
  }

  void setEMA(float alpha) {
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;
    emaAlpha = alpha;
  }

  void setCalibration(float gain, float offset) {
    kGain = gain; kOffset = offset;
  }

  void loadCalibration() {
    Preferences p; p.begin("protoetch", true);
    kGain   = p.getFloat("t_gain", 1.0f);
    kOffset = p.getFloat("t_offs", 0.0f);
    p.end();
  }

  void saveCalibration(float gain, float offset) {
    Preferences p; p.begin("protoetch", false);
    p.putFloat("t_gain", gain);
    p.putFloat("t_offs", offset);
    p.end();
    kGain = gain; kOffset = offset;
  }

  bool subscribe(Callback cb) {
    for (auto &slot : listeners) {
      if (slot == nullptr) { slot = cb; return true; }
    }
    return false;
  }

  Sample latest() { return last; }

  bool healthy() {
    // geldig als er in de afgelopen 5*period een valide sample was
    return last.valid && (millis() - last.ts) <= periodMs * 5UL;
  }

  Error lastError() { return gErr; }
  Stats stats()     { return gSt;  }

  void forceSample() { tNext = 0; } // asap conversie starten

  void tick() {
    if (!hasDev) return;

    const uint32_t now = millis();
    switch (st) {

      case IDLE:
        if ((int32_t)(now - tNext) >= 0) { // tijd om te meten
          st = START_CONV;
        }
        break;

      case START_CONV:
        ds.requestTemperaturesByAddress(addr); // start conversie
        tConv = now;
        st = WAIT_CONV;
        break;

      case WAIT_CONV:
        if (ds.isConversionComplete()) {
          st = READ;
        } else if (now - tConv > MAX_TIMEOUT_MS) {
          // timeout
          last.valid = false; last.ts = now;
          gErr = TIMEOUT; gSt.timeouts++;
          notify(last);
          scheduleNext(now, /*ok=*/false);
          st = IDLE;
        }
        break;

      case READ: {
        float tC = NAN;
        if (!readWithCRC(tC)) {
          // CRC / read fail
          last.valid = false; last.ts = now;
          gErr = CRC_FAIL; gSt.crc++;
          notify(last);
          scheduleNext(now, /*ok=*/false);
          st = IDLE;
          break;
        }

        bool ok = !(tC == DEVICE_DISCONNECTED_C || isnan(tC) || tC < -55.0f || tC > 125.0f);
        if (ok) {
          if (emaAlpha > 0.0f) {
            if (isnan(emaValue)) emaValue = tC;
            emaValue = emaAlpha * tC + (1.0f - emaAlpha) * emaValue;
            tC = emaValue;
          }
          // kalibratie
          float tCal = kGain * tC + kOffset;

          last.c     = tCal;
          last.ts    = now;
          last.valid = true;
          gErr       = OK;
          gSt.ok++;
          notify(last);
          scheduleNext(now, /*ok=*/true);
        } else {
          last.valid = false; last.ts = now;
          gErr = RANGE_FAIL; gSt.range++;
          notify(last);
          scheduleNext(now, /*ok=*/false);
        }
        st = IDLE;
        break;
      }
    } // switch
  }
} // namespace TempSensor