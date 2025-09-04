#pragma once
#include <Arduino.h>

namespace TempSensor {

/** Initialise OneWire + DallasTemperature, non-blocking mode. */
void begin();

/** Call periodically (e.g. every loop). Triggers conversions at TS_DEFAULT_PERIOD_MS. */
void update();

/** Latest temperature in °C, or NAN if not available yet. */
float latestC();

/** True if we have seen at least one valid reading. */
bool healthy();

} // namespace TempSensor