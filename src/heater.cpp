#include "config.h"
#include "heater.h"

namespace { bool state = false; }

namespace Heater {

  void begin() {
    pinMode(PIN_HEATER_RELAY, OUTPUT);
    digitalWrite(PIN_HEATER_RELAY, HEATER_RELAY_OFF); // safe on boot
    state = false;
  }

  void setEnabled(bool on) {
    state = on;
    digitalWrite(PIN_HEATER_RELAY, on ? HEATER_RELAY_ON : HEATER_RELAY_OFF);
  }

  bool isEnabled() {
    return state;
  }

  void emergencyOff() {
    state = false;
    digitalWrite(PIN_HEATER_RELAY, HEATER_RELAY_OFF);
  }
}