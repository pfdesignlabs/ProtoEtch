#pragma once
#include <Arduino.h>

namespace Pump {
  // Pas aan indien nodig
  constexpr int  PIN      = 25;      // GPIO25 = Pump CTRL (jouw mapping)
  constexpr int  LEDC_CH  = 0;       // vrij LEDC kanaal
  constexpr int  LEDC_HZ  = 20000;   // ~20 kHz
  constexpr int  LEDC_BITS= 8;       // 0..255 duty
  constexpr uint8_t ON_DUTY = 255;   // volle kracht

  void begin();
  void on();
  void off();
  void setDuty(uint8_t duty);     // 0..255
  void onFor(uint32_t ms);        // zet aan en stop automatisch na ms
  void update();                  // call in loop()
  bool isOn();
}