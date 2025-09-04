#include <Arduino.h>
#include "config.h"
#include "sensor_ds18b20.h"
#include "heater_controller.h"
#include "ui/display_ui.h"

/*
  Minimal integration loop:
  - Reads DS18B20 non-blocking
  - Feeds heater controller (bang-bang w/ hysteresis & hold times)
  - Renders values on TFT_eSPI UI
*/

void setup() {
  Serial.begin(115200);
  delay(200);

  LOGI("\n[ProtoEtch] Booting...\n");

  TempSensor::begin();
  HeaterCtl::begin();
  DisplayUI::begin();

  // Example: adjust defaults at runtime if desired
  // HeaterCtl::setSetpoint(45.0f);
  // HeaterCtl::setHysteresis(0.8f);
}

void loop() {
  // 1) Update sensor state machine
  TempSensor::update();

  // 2) Consume sensor value
  const float tC = TempSensor::latestC();
  HeaterCtl::tick(tC);

  // 3) UI refresh (simple timed refresh)
  static uint32_t lastUi = 0;
  const uint32_t now = millis();
  if (now - lastUi >= 250) {
    DisplayUI::update(
      tC,
      HeaterCtl::getSetpointC(),
      HeaterCtl::relayState()
    );
    lastUi = now;
  }
}