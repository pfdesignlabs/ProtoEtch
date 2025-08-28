#pragma once
#include <Arduino.h>

namespace TempSensor {

  struct Sample {
    float     c    = NAN;    // laatste (gecorrigeerde) temperatuur in °C
    uint32_t  ts   = 0;      // timestamp (millis)
    bool      valid= false;  // true = waarde betrouwbaar
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
  void tick();                          // call in loop()

  // Config
  void setPeriod(uint32_t ms);          // default TS_DEFAULT_PERIOD_MS
  void setResolution(uint8_t bits);     // 9..12, default TS_RES
  void setEMA(float alpha);             // 0=uit, bv. 0.2 voor smoothing

  // Kalibratie (gain/offset), met NVS helpers
  void setCalibration(float gain, float offset);
  void loadCalibration();               // uit NVS ("protoetch")
  void saveCalibration(float gain, float offset); // naar NVS

  // API
  bool   subscribe(Callback cb);        // max 4 listeners
  Sample latest();                      // cached, non-blocking
  bool   healthy();                     // recent geldige sample?
  Error  lastError();
  Stats  stats();

  // Tools
  void forceSample();                   // asap nieuwe meting starten
}