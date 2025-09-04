#include "heater_controller.h"
#include "config.h"

namespace {
  struct Cfg {
    float    setpointC   = HEATER_SETPOINT_C;
    float    hysteresisC = HEATER_HYST_C;
    float    maxTempC    = HEATER_MAX_TEMP_C;
    uint32_t minOnMs     = HEATER_MIN_ON_MS;
    uint32_t minOffMs    = HEATER_MIN_OFF_MS;
    bool     enabled     = true;
  } cfg;

  struct St {
    bool     relayOn    = false;
    uint32_t lastChange = 0;
  } st;

  inline void driveRelay(bool on) {
    digitalWrite(PIN_HEATER_RELAY, on ? HEATER_RELAY_ON : HEATER_RELAY_OFF);
    st.relayOn   = on;
    st.lastChange= millis();
  }
  inline bool canOn(uint32_t now)  { return (now - st.lastChange) >= cfg.minOffMs; }
  inline bool canOff(uint32_t now) { return (now - st.lastChange) >= cfg.minOnMs;  }
}

namespace HeaterCtl {

void begin() {
  pinMode(PIN_HEATER_RELAY, OUTPUT);
  driveRelay(false);
  LOGI("[HeaterCtl] Relay pin=%d, active_high=%d\n", PIN_HEATER_RELAY, HEATER_ACTIVE_HIGH);
}

void setSetpoint(float c)  { cfg.setpointC   = constrain(c, 20.0f, 70.0f); }
void setHysteresis(float c){ cfg.hysteresisC = constrain(c, 0.2f, 5.0f);   }

float getSetpointC()    { return cfg.setpointC; }
float getHysteresisC()  { return cfg.hysteresisC; }

void enable(bool en) {
  cfg.enabled = en;
  if (!en && st.relayOn && canOff(millis())) driveRelay(false);
}
bool enabled() { return cfg.enabled; }

void tick(float tc) {
  const uint32_t now = millis();

  // Safety and preconditions
  if (!cfg.enabled || isnan(tc) || tc >= cfg.maxTempC) {
    if (st.relayOn && canOff(now)) driveRelay(false);
    return;
  }

  const float low  = cfg.setpointC - (cfg.hysteresisC * 0.5f);
  const float high = cfg.setpointC + (cfg.hysteresisC * 0.5f);

  if (!st.relayOn) {
    if (tc < low && canOn(now))  driveRelay(true);
  } else {
    if (tc > high && canOff(now)) driveRelay(false);
  }
}

bool relayState() { return st.relayOn; }

} // namespace HeaterCtl