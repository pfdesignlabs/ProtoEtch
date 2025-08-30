#include "heater_controller.h"
#include "config.h"

namespace {
  struct Cfg {
    float setpointC    = HEATER_SETPOINT_C;
    float hysteresisC  = HEATER_HYST_C;
    float maxTempC     = HEATER_MAX_TEMP_C;
    uint32_t minOnMs   = HEATER_MIN_ON_MS;
    uint32_t minOffMs  = HEATER_MIN_OFF_MS;
    bool enabled       = true;
  } cfg;

  bool relayOn = false;
  uint32_t lastSwitchMs = 0;

  inline void driveRelay(bool on) {
    relayOn = on;
    const bool pinLevel = HEATER_ACTIVE_HIGH ? on : !on;
    digitalWrite(PIN_HEATER_RELAY, pinLevel ? HIGH : LOW);
  }

  inline bool canTurnOn(uint32_t now)  { return (now - lastSwitchMs) >= cfg.minOffMs; }
  inline bool canTurnOff(uint32_t now) { return (now - lastSwitchMs) >= cfg.minOnMs; }

  void controlLoop(float tC) {
    const uint32_t now = millis();

    // If disabled or invalid reading -> force OFF
    if (!cfg.enabled || isnan(tC)) {
      if (relayOn && canTurnOff(now)) {
        driveRelay(false);
        lastSwitchMs = now;
      } else if (!relayOn) {
        driveRelay(false);
      }
      return;
    }

    // Over-temperature safety
    if (tC >= cfg.maxTempC) {
      if (relayOn && canTurnOff(now)) {
        driveRelay(false);
        lastSwitchMs = now;
      } else if (!relayOn) {
        driveRelay(false);
      }
      return;
    }

    // Bang-bang with hysteresis + min hold times
    const float lo = cfg.setpointC - cfg.hysteresisC;
    const float hi = cfg.setpointC + cfg.hysteresisC;

    if (!relayOn) {
      if (tC < lo && canTurnOn(now)) {
        driveRelay(true);
        lastSwitchMs = now;
      }
    } else { // relayOn
      if (tC > hi && canTurnOff(now)) {
        driveRelay(false);
        lastSwitchMs = now;
      }
    }
  }
}

namespace HeaterCtl {

  void begin() {
    pinMode(PIN_HEATER_RELAY, OUTPUT);
    // Ensure relay is OFF at boot
    driveRelay(false);
    lastSwitchMs = millis();
  }

  void tick(float currentTempC) { controlLoop(currentTempC); }

  void setEnabled(bool en) {
    cfg.enabled = en;
    if (!en) {
      // force off immediately if allowed by min-on
      const uint32_t now = millis();
      if (relayOn && canTurnOff(now)) {
        driveRelay(false);
        lastSwitchMs = now;
      }
    }
  }
  bool enabled() { return cfg.enabled; }

  bool isOn() { return relayOn; }

  void setSetpoint(float c)  { cfg.setpointC   = constrain(c, 20.0f, 70.0f); }
  float getSetpoint()        { return cfg.setpointC; }

  void setHysteresis(float c){ cfg.hysteresisC = constrain(c, 0.2f, 5.0f); }
  float getHysteresis()      { return cfg.hysteresisC; }

  void setMaxTemp(float c)   { cfg.maxTempC    = constrain(c, 30.0f, 80.0f); }
  float getMaxTemp()         { return cfg.maxTempC; }

  void setMinOnMs(uint32_t ms)  { cfg.minOnMs  = ms; }
  void setMinOffMs(uint32_t ms) { cfg.minOffMs = ms; }
  uint32_t getMinOnMs()         { return cfg.minOnMs; }
  uint32_t getMinOffMs()        { return cfg.minOffMs; }
}