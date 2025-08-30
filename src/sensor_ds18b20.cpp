#include "sensor_ds18b20.h"
#include "config.h"

#include <OneWire.h>
#include <DallasTemperature.h>

namespace {
  OneWire           ow(TS_PIN);
  DallasTemperature dt(&ow);
  DeviceAddress     rom{};
  bool              hasDevice = false;

  uint32_t  lastKickMs = 0;
  bool      waiting    = false;
  float     lastC      = NAN;

  void kickConversion() {
    if (!hasDevice) return;
    dt.requestTemperaturesByAddress(rom);
    waiting   = true;
    lastKickMs= millis();
  }
}

namespace TempSensor {

void begin() {
  dt.begin();
  dt.setWaitForConversion(false); // non-blocking
  if (dt.getAddress(rom, 0)) {
    hasDevice = true;
    dt.setResolution(rom, TS_RES);
    LOGI("[Temp] DS18B20 found, res=%d-bit\n", TS_RES);
    kickConversion();
  } else {
    hasDevice = false;
    LOGW("[Temp] No DS18B20 found on pin %d\n", TS_PIN);
  }
}

void update() {
  const uint32_t now = millis();
  if (!hasDevice) return;

  // If waiting, check if conversion completed or timed out
  if (waiting) {
    if (dt.isConversionComplete()) {
      float t = dt.getTempC(rom);
      if (t != DEVICE_DISCONNECTED_C && t > -55.0f && t < 125.0f) {
        lastC = t;
      } else {
        lastC = NAN;
      }
      waiting = false;
      // schedule next kick after period
      if (now - lastKickMs >= TS_DEFAULT_PERIOD_MS) kickConversion();
    } else if (now - lastKickMs > TS_TIMEOUT_MS) {
      LOGW("[Temp] Conversion timeout\n");
      waiting = false;
      // Try again next cycle
    }
  } else {
    // Not waiting: time to start next conversion?
    if (now - lastKickMs >= TS_DEFAULT_PERIOD_MS) kickConversion();
  }
}

float latestC()   { return lastC; }
bool  healthy()   { return !isnan(lastC); }

} // namespace TempSensor