// Simple PWM pump driver using ESP32 LEDC
#include <Arduino.h>
#include "pump.h"

namespace {
  uint32_t g_offAt = 0;   // millis deadline for auto-off
  bool     g_on    = false;
}

void Pump::begin() {
  ledcSetup(LEDC_CH, LEDC_HZ, LEDC_BITS);
  ledcAttachPin(PIN, LEDC_CH);
  setDuty(0);
  g_on = false;
  g_offAt = 0;
}

void Pump::setDuty(uint8_t duty) {
  ledcWrite(LEDC_CH, duty);
  g_on = duty > 0;
}

void Pump::on()  { setDuty(ON_DUTY); }
void Pump::off() { setDuty(0);       }

void Pump::onFor(uint32_t ms) {
  on();
  g_offAt = millis() + ms;
}

void Pump::update() {
  if (g_offAt && millis() >= g_offAt) {
    off();
    g_offAt = 0;
  }
}

bool Pump::isOn() { return g_on; }
