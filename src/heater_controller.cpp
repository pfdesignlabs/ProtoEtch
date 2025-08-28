#include <Preferences.h>
#include "config.h"
#include "heater.h"
#include "sensor_ds18b20.h"
#include "heater_controller.h"

namespace {
  // Runtime config (defaults from config.h)
  bool     gEnabled    = true;
  float    gSet        = HEATER_SETPOINT_C;
  float    gHyst       = HEATER_HYST_C;
  float    gMaxCut     = HEATER_MAX_TEMP_C;
  uint32_t gMinOnMs    = HEATER_MIN_ON_MS;
  uint32_t gMinOffMs   = HEATER_MIN_OFF_MS;

  // State
  HeaterCtl::Status st{};
  uint32_t onSince  = 0;
  uint32_t offSince = 0;

  inline bool minOnActive(uint32_t now)  { return Heater::isEnabled() && (now - onSince  < gMinOnMs);  }
  inline bool minOffActive(uint32_t now) { return !Heater::isEnabled()&& (now - offSince < gMinOffMs); }

  void setRelay(bool on) {
    if (on == Heater::isEnabled()) return;
    Heater::setEnabled(on);
    uint32_t now = millis();
    if (on) onSince = now; else offSince = now;
  }

  // Clamp incoming values to sane ranges
  inline float clampSet(float c)  { return constrain(c, 5.0f, 70.0f); }
  inline float clampHys(float h)  { return constrain(h, 0.2f, 5.0f);  }
  inline float clampMax(float c)  { return constrain(c, 30.0f, 80.0f);}
  inline uint32_t clampHold(uint32_t ms){ return constrain(ms, 1000UL, 600000UL); }

  void updateRemainTimes(uint32_t now) {
    if (Heater::isEnabled()) {
      uint32_t elapsed = now - onSince;
      st.remainMinOnMs  = (elapsed < gMinOnMs) ? (gMinOnMs - elapsed) : 0;
      st.remainMinOffMs = 0;
    } else {
      uint32_t elapsed = now - offSince;
      st.remainMinOffMs = (elapsed < gMinOffMs) ? (gMinOffMs - elapsed) : 0;
      st.remainMinOnMs  = 0;
    }
  }
}

namespace HeaterCtl {

  void begin() {
    Heater::begin();
    offSince = millis();         // start OFF
    setRelay(false);

    st.setpointC   = gSet;
    st.hysteresisC = gHyst;
    st.currentC    = NAN;
    st.relayOn     = false;
    st.reason      = Reason::Idle;
    st.sinceMs     = millis();
    st.remainMinOnMs = st.remainMinOffMs = 0;

    // Load persisted settings
    loadSettingsFromNVS();
  }

  // Runtime setters
  void setEnabled(bool en)      { gEnabled = en; }
  void setSetpoint(float c)     { gSet  = clampSet(c);  st.setpointC = gSet; }
  void setHysteresis(float c)   { gHyst = clampHys(c);  st.hysteresisC = gHyst; }
  void setMinOnOff(uint32_t minOnMs, uint32_t minOffMs) {
    gMinOnMs  = clampHold(minOnMs);
    gMinOffMs = clampHold(minOffMs);
  }
  void setMaxTempCutoff(float c){ gMaxCut = clampMax(c); }

  // Persistence (NVS)
  void loadSettingsFromNVS() {
    Preferences p; p.begin(NVS_NS_PROTOETCH, true);
    if (p.isKey(NVS_KEY_HT_SET))   { gSet  = clampSet(p.getFloat(NVS_KEY_HT_SET,  gSet)); }
    if (p.isKey(NVS_KEY_HT_HYST))  { gHyst = clampHys(p.getFloat(NVS_KEY_HT_HYST, gHyst)); }
    if (p.isKey(NVS_KEY_HT_EN))    { gEnabled = p.getUChar(NVS_KEY_HT_EN, gEnabled?1:0) != 0; }
    if (p.isKey(NVS_KEY_HT_MAX))   { gMaxCut = clampMax(p.getFloat(NVS_KEY_HT_MAX, gMaxCut)); }
    if (p.isKey(NVS_KEY_HT_MINON)) { gMinOnMs  = clampHold(p.getULong(NVS_KEY_HT_MINON,  gMinOnMs)); }
    if (p.isKey(NVS_KEY_HT_MINOFF)){ gMinOffMs = clampHold(p.getULong(NVS_KEY_HT_MINOFF, gMinOffMs)); }
    p.end();
    st.setpointC = gSet; st.hysteresisC = gHyst;
  }

  void saveSettingsToNVS() {
    Preferences p; p.begin(NVS_NS_PROTOETCH, false);
    p.putFloat(NVS_KEY_HT_SET,   gSet);
    p.putFloat(NVS_KEY_HT_HYST,  gHyst);
    p.putUChar(NVS_KEY_HT_EN,    gEnabled ? 1 : 0);
    p.putFloat(NVS_KEY_HT_MAX,   gMaxCut);
    p.putULong(NVS_KEY_HT_MINON, gMinOnMs);
    p.putULong(NVS_KEY_HT_MINOFF,gMinOffMs);
    p.end();
  }

  void setAndSave(float setC, float hystC, bool en) {
    setSetpoint(setC);
    setHysteresis(hystC);
    setEnabled(en);
    saveSettingsToNVS();
  }

  // Inspect
  HeaterCtl::Status getStatus() {
    st.relayOn = Heater::isEnabled();
    return st;
  }

  // Control loop
  void tick() {
    const uint32_t now = millis();

    auto s = TempSensor::latest();
    st.currentC = s.c;
    updateRemainTimes(now);

    // Failsafes first
    if (!TempSensor::healthy()) {
      if (Heater::isEnabled()) Heater::emergencyOff();
      if (st.reason != Reason::SensorStale) { st.reason = Reason::SensorStale; st.sinceMs = now; }
      return;
    }
    if (!isnan(st.currentC) && st.currentC >= gMaxCut) {
      if (Heater::isEnabled()) Heater::emergencyOff();
      if (st.reason != Reason::OverTempCutoff) { st.reason = Reason::OverTempCutoff; st.sinceMs = now; }
      return;
    }

    // Controller disabled or setpoint invalid
    if (!gEnabled || gSet <= 0.0f) {
      if (Heater::isEnabled()) setRelay(false);
      if (st.reason != Reason::Disabled) { st.reason = Reason::Disabled; st.sinceMs = now; }
      return;
    }

    // Respect min on/off holds
    if (minOnActive(now))  { if (st.reason != Reason::MinOnHold)  { st.reason = Reason::MinOnHold;  st.sinceMs = now; } return; }
    if (minOffActive(now)) { if (st.reason != Reason::MinOffHold) { st.reason = Reason::MinOffHold; st.sinceMs = now; } return; }

    // Hysteresis control
    const float low  = gSet - gHyst;
    const float high = gSet + gHyst;

    if (isnan(st.currentC)) {
      if (Heater::isEnabled()) setRelay(false);
      if (st.reason != Reason::SensorStale) { st.reason = Reason::SensorStale; st.sinceMs = now; }
      return;
    }

    if (st.currentC <= low) {
      setRelay(true);
      if (st.reason != Reason::BelowBand) { st.reason = Reason::BelowBand; st.sinceMs = now; }
      return;
    }
    if (st.currentC >= high) {
      setRelay(false);
      if (st.reason != Reason::AboveBand) { st.reason = Reason::AboveBand; st.sinceMs = now; }
      return;
    }

    if (st.reason != Reason::Idle) { st.reason = Reason::Idle; st.sinceMs = now; }
  }

  // Tiny serial CLI (newline-terminated commands)
  //  HEAT EN 1/0
  //  HEAT SET <floatC>
  //  HEAT HYS <floatC>
  //  HEAT SAVE
  //  HEAT LOAD
  //  HEAT MINS <onMs> <offMs>
  //  HEAT MAX <floatC>
  //  HEAT STATUS
  void serialCLI() {
    if (!Serial.available()) return;
    String line = Serial.readStringUntil('\n');
    line.trim(); line.toUpperCase();

    if (line.startsWith("HEAT EN ")) {
      bool en = line.endsWith("1");
      setEnabled(en);
      Serial.printf("[HEAT] enabled=%d\n", en);
    } else if (line.startsWith("HEAT SET ")) {
      float v = line.substring(9).toFloat();
      setSetpoint(v);
      Serial.printf("[HEAT] setpoint=%.2f C\n", getStatus().setpointC);
    } else if (line.startsWith("HEAT HYS ")) {
      float v = line.substring(9).toFloat();
      setHysteresis(v);
      Serial.printf("[HEAT] hysteresis=%.2f C\n", getStatus().hysteresisC);
    } else if (line == "HEAT SAVE") {
      saveSettingsToNVS();
      Serial.println("[HEAT] settings saved");
    } else if (line == "HEAT LOAD") {
      loadSettingsFromNVS();
      Serial.println("[HEAT] settings loaded");
    } else if (line.startsWith("HEAT MINS ")) {
      int sp1 = line.indexOf(' ', 10);
      uint32_t mon = line.substring(10, sp1).toInt();
      uint32_t mof = line.substring(sp1+1).toInt();
      setMinOnOff(mon, mof);
      Serial.printf("[HEAT] minOn=%u ms  minOff=%u ms\n", mon, mof);
    } else if (line.startsWith("HEAT MAX ")) {
      float v = line.substring(9).toFloat();
      setMaxTempCutoff(v);
      Serial.printf("[HEAT] cutoff=%.1f C\n", v);
    } else if (line == "HEAT STATUS") {
      auto s = getStatus();
      Serial.printf("[HEAT] T=%.2f C  set=%.2f C  hyst=%.2f C  on=%d  reason=%u  remainOn=%u ms  remainOff=%u ms\n",
        s.currentC, s.setpointC, s.hysteresisC, s.relayOn, (unsigned)s.reason, s.remainMinOnMs, s.remainMinOffMs);
    }
  }
}