// Minimal relay HAL (active-LOW per board design)
#include <Arduino.h>
#include "heater.h"

namespace {
  bool g_on = false;
}

static inline void writeActiveLow(int pin, bool active) {
  digitalWrite(pin, active ? LOW : HIGH);
}

void Heater::begin() {
  pinMode(PIN, OUTPUT);
  writeActiveLow(PIN, false);
  g_on = false;
}

void Heater::set(bool on) {
  writeActiveLow(PIN, on);
  g_on = on;
}

bool Heater::get() { return g_on; }
