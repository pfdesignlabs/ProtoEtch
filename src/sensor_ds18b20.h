#pragma once
#include <Arduino.h>

namespace TempSensor {

  struct Sample {
    float     c    = NAN;    // last calibrated temperature (°C)
    uint32_t  ts   = 0;      // millis() timestamp
    bool      valid= false;  // true if value is trustworthy
  };

  enum Error {
    OK = 0,
    NO_DEVICE,
    TIMEOUT,
    CRC_FAIL,
    RANGE_FAIL
  };

  struct Stats {
    uint32_t ok       = 0;
    uint32_t timeouts = 0;
    uint32_t crc      = 0;
    uint32_t range    = 0;
  };

  using Callback = void(*)(const Sample&);

  // Lifecycle
  void begin();
  void tick();                          // call each loop()

  // Configuration
  void setPeriod(uint32_t ms);          // default TS_DEFAULT_PERIOD_MS
  void setResolution(uint8_t bits);     // 9..12, default TS_RES
  void setEMA(float alpha);             // 0=off, e.g., 0.2 for smoothing

  // Calibration (gain/offset) with NVS helpers
  void setCalibration(float gain, float offset);
  void loadCalibration();
  void saveCalibration(float gain, float offset);

  // API
  bool   subscribe(Callback cb);        // up to 4 listeners
  Sample latest();                      // cached, non-blocking
  bool   healthy();                     // recent valid sample?
  Error  lastError();
  Stats  stats();

  // Tools
  void forceSample();                   // trigger a new sample asap
}