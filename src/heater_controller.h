#pragma once
#include <Arduino.h>

// Simple bang-bang controller with hysteresis + min on/off hold
namespace HeaterCtl {

  // Lifecycle
  void begin();                  // configures pin & forces relay OFF
  void tick(float currentTempC); // run control loop once (call often)

  // Aliases for convenience/back-compat
  inline void update(float c) { tick(c); }

  // Enable/disable
  void setEnabled(bool en);
  bool enabled();

  // Relay state readback
  bool isOn();

  // Setpoint / hysteresis
  void setSetpoint(float c);
  float getSetpoint();
  // Back-compat helper (same as getSetpoint())
  inline float getSetpointC() { return getSetpoint(); }

  void setHysteresis(float c);
  float getHysteresis();

  // Safety limits
  void setMaxTemp(float c);
  float getMaxTemp();

  // Minimum hold times (ms) to protect relay
  void setMinOnMs(uint32_t ms);
  void setMinOffMs(uint32_t ms);
  uint32_t getMinOnMs();
  uint32_t getMinOffMs();
}