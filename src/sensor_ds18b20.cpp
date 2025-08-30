#include "sensor_ds18b20.h"
#include "config.h"
#include <OneWire.h>
#include <DallasTemperature.h>

namespace {
  OneWire oneWire(TS_PIN);
  DallasTemperature dallas(&oneWire);
  DeviceAddress rom{};

  // Config
  uint32_t periodMs = TS_DEFAULT_PERIOD_MS;
  float emaAlpha = 0.25f;
  float calGain  = 1.0f;
  float calOffs  = 0.0f;

  // State
  enum Phase { IDLE, CONVERTING } phase = IDLE;
  uint32_t lastKick = 0;
  uint32_t convertStart = 0;

  TempSensor::Sample smp;  // filtered sample
  bool haveDevice = false;

  inline bool conversionReady() { return dallas.isConversionComplete(); }

  void startConversion() {
    dallas.requestTemperaturesByAddress(rom);
    convertStart = millis();
    phase = CONVERTING;
  }

  void finishConversion() {
    float t = dallas.getTempC(rom);
    bool ok = (t != DEVICE_DISCONNECTED_C) && t > -55.0f && t < 125.0f && !isnan(t);
    if (ok) {
      t = calGain * t + calOffs;
      if (isnan(smp.valueC)) smp.valueC = t;                  // seed
      smp.valueC = emaAlpha * t + (1.0f - emaAlpha) * smp.valueC;
      smp.ok = true;
      smp.ageMs = 0;
    } else {
      smp.ok = false; // keep last value, only age
    }
    phase = IDLE;
  }
} // anon

namespace TempSensor {

void begin() {
  dallas.begin();
  if (dallas.getAddress(rom, 0)) {
    haveDevice = true;
    dallas.setResolution(rom, TS_RES);
    dallas.setWaitForConversion(false); // non-blocking
    LOGI("[Temp] DS18B20 found, res=%d-bit\n", TS_RES);
  } else {
    haveDevice = false;
    LOGW("[Temp] No DS18B20 detected on pin %d\n", TS_PIN);
  }
  smp = {}; // reset to defaults
}

void poll() {
  const uint32_t now = millis();

  // Age sample while idle
  if (phase == IDLE && smp.ageMs < 0xFFFFFFFFu) {
    uint32_t dt = now - lastKick;
    smp.ageMs = (dt > smp.ageMs) ? dt : smp.ageMs + dt; // simple aging
  }

  if (!haveDevice) return;

  switch (phase) {
    case IDLE:
      if (now - lastKick >= periodMs) {
        lastKick = now;
        startConversion();
      }
      break;

    case CONVERTING: {
      const uint32_t elapsed = now - convertStart;
      if (conversionReady()) {
        finishConversion();
      } else if (elapsed > TS_TIMEOUT_MS) {
        LOGW("[Temp] Conversion timeout (%lums)\n", elapsed);
        phase = IDLE; // retry next cycle
      }
    } break;
  }
}

Sample latest() { return smp; }

void setPeriodMs(uint32_t ms) { periodMs = ms < 100 ? 100 : ms; }
void setEMA(float a)          { emaAlpha = constrain(a, 0.0f, 1.0f); }
void setCalibration(float g, float o) { calGain = g; calOffs = o; }

} // namespace TempSensor