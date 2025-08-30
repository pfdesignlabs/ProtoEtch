#pragma once
#include <Arduino.h>

namespace Heater {
  // Initialize the relay pin and logic level (true = active HIGH, false = active LOW)
  void begin(uint8_t pin, bool activeHigh);

  // Command relay ON/OFF
  void command(bool on);

  // Query current relay state as tracked by the HAL
  bool isOn();
}