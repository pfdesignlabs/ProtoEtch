#pragma once
#include <Arduino.h>

namespace TempSensor {

struct Sample {
  float     valueC = NAN;  // filtered temperature (°C)
  uint32_t  ageMs  = 0;    // ms since last successful update
  bool      ok     = false;
};

void begin();                   // init the 1-Wire & Dallas device
void poll();                    // call frequently (non-blocking state machine)
Sample latest();                // last filtered sample

// Optional tuning
void setPeriodMs(uint32_t ms);  // default TS_DEFAULT_PERIOD_MS
void setEMA(float alpha);       // 0..1 (0=no smoothing, 1=all smoothing)
void setCalibration(float gain, float offset); // y = gain*x + offset

} // namespace