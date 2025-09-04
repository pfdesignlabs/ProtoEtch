#pragma once
#include <Arduino.h>

namespace HeaterCtl {

/** Configure relay pin and default params (OFF at boot). */
void begin();

/** Set target temperature (°C). Clamped to a safe range. */
void setSetpoint(float c);

/** Set total hysteresis band (°C), e.g. 0.8 → ±0.4 around setpoint. */
void setHysteresis(float c);

/** Read back current configuration. */
float getSetpointC();
float getHysteresisC();

/** Arm/disarm the controller output. Disabling forces relay OFF (with min-off hold). */
void enable(bool en);
bool enabled();

/** Feed the current temperature (°C). NAN is treated as fault → relay OFF. */
void tick(float currentTempC);

/** Current relay state (true = ON). */
bool relayState();

} // namespace HeaterCtl