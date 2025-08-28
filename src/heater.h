#pragma once
#include <Arduino.h>

namespace Heater {
  void begin();                // init relay pin (safe OFF)
  void setEnabled(bool on);    // switch relay
  bool isEnabled();            // current relay state
  void emergencyOff();         // hard OFF (failsafe)
}