#pragma once
#include <Arduino.h>
#include "pump.h"

namespace Heater {
  // Jouw mapping: Heater Relay IN = GPIO26, Active-LOW
  constexpr int PIN = 26;

  void begin();
  void set(bool on);  // on=true -> relais aantrekken (active-LOW)
  bool get();
}