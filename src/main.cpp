#include <Arduino.h>
#include "config.h"
#include "sensor_ds18b20.h"
#include "heater_controller.h"
#include "ui/display_ui.h"
#include "pump.h"

/*
  ProtoEtch main loop
  - Reads DS18B20 non-blocking
  - Feeds heater controller (bang-bang w/ hysteresis & hold times)
  - Triggers pump for 30 s on heater relay rising edge (non-blocking)
  - Renders values on TFT_eSPI UI
*/

void setup() {
  Serial.begin(115200);
  delay(200);

  LOGI("\n[ProtoEtch] Booting...\n");

  TempSensor::begin();
  HeaterCtl::begin();
  Pump::begin();          // init pomp driver (LEDC, pin 25 bv.)
  DisplayUI::begin();     // init TFT and draw static UI
}

void loop() {
  // 1) Sensor update
  TempSensor::update();

  // 2) Regelaar
  const float tC = TempSensor::latestC();
  HeaterCtl::tick(tC);

  // 3) Rising-edge detectie op heater-relais -> pomp 30 s aan
  static bool lastRelay = false;
  const bool relayNow = HeaterCtl::relayState();   // true = aan
  if (relayNow && !lastRelay) {
    Pump::onFor(30000); // 30 s non-blocking
  }
  lastRelay = relayNow;

  // 4) Pomp timer afhandelen
  Pump::update();

  // 5) UI refresh
  static uint32_t lastUi = 0;
  const uint32_t now = millis();
  if (now - lastUi >= 250) {
    DisplayUI::update(
      tC,
      HeaterCtl::getSetpointC(),
      HeaterCtl::relayState(),
      Pump::isOn()
    );
    lastUi = now;
  }
}
