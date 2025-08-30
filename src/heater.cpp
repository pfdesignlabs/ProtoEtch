#include "heater.h"

namespace {
  uint8_t g_pin        = 255;
  bool    g_activeHigh = true;
  bool    g_isOn       = false;
  bool    g_inited     = false;
}

void Heater::begin(uint8_t pin, bool activeHigh) {
  g_pin        = pin;
  g_activeHigh = activeHigh;
  pinMode(g_pin, OUTPUT);
  // Default OFF
  g_isOn = false;
  digitalWrite(g_pin, g_activeHigh ? LOW : HIGH);
  g_inited = true;
}

void Heater::command(bool on) {
  if (!g_inited) return;
  g_isOn = on;
  digitalWrite(g_pin, g_activeHigh ? (on ? HIGH : LOW)
                                   : (on ? LOW  : HIGH));
}

bool Heater::isOn() {
  return g_isOn;
}