#pragma once
#include <Arduino.h>

namespace HeaterCtl {

  enum class Reason : uint8_t {
    Idle,
    BelowBand,
    AboveBand,
    MinOnHold,
    MinOffHold,
    SensorStale,
    OverTempCutoff,
    Disabled
  };

  struct Status {
    float     setpointC   = 0.0f;
    float     hysteresisC = 0.5f;
    float     currentC    = NAN;
    bool      relayOn     = false;
    Reason    reason      = Reason::Idle;
    uint32_t  sinceMs     = 0;
    // remaining hold times (0 if not applicable)
    uint32_t  remainMinOnMs  = 0;
    uint32_t  remainMinOffMs = 0;
  };

  // Lifecycle
  void begin();
  void tick();

  // Runtime config
  void setEnabled(bool en);
  void setSetpoint(float c);     // °C
  void setHysteresis(float c);   // °C
  void setMinOnOff(uint32_t minOnMs, uint32_t minOffMs);
  void setMaxTempCutoff(float c);// °C

  // Persistent settings (NVS)
  void loadSettingsFromNVS();
  void saveSettingsToNVS();
  void setAndSave(float setC, float hystC, bool en);

  // Inspect
  Status getStatus();

  // Optional: tiny Serial CLI to tune at runtime
  void serialCLI();
}