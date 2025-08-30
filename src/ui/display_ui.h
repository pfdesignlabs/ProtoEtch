#pragma once
#include <Arduino.h>

namespace DisplayUI {

/** Init TFT (TFT_eSPI), set rotation, draw static chrome. */
void begin();

/** Refresh dynamic values (temp, setpoint, heater). */
void update(float tempC, float setpointC, bool heaterOn);

} // namespace DisplayUI