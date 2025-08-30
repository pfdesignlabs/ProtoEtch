#pragma once
#include <TFT_eSPI.h>
#include <Arduino.h>

// Simple status DTO the app fills in
struct UIStatus {
  float    tempC   = NAN;
  bool     heaterOn = false;
  bool     pumpOn   = false;
  bool     wifiOk   = false;
};

namespace DisplayUI {
  void begin();                // init TFT + theme
  void render(const UIStatus&);// draw the screen (idempotent)
}