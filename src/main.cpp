#include <Arduino.h>
#include "config.h"
#include "sensor_ds18b20.h"

// If you are not using the display in this test, you can omit display headers.
// Keep it super small and just print to Serial.

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\n[ProtoEtch] TempSensor minimal bring-up");

  TempSensor::begin();
  TempSensor::setPeriodMs(1000);
  TempSensor::setEMA(0.25f);
}

void loop() {
  TempSensor::poll();

  static uint32_t t0 = 0;
  if (millis() - t0 >= 1000) {
    t0 = millis();
    auto s = TempSensor::latest();
    if (s.ok) {
      Serial.printf("[Temp] %0.2f C (age %lums)\n", s.valueC, s.ageMs);
    } else {
      Serial.println("[Temp] read failed or not ready");
    }
  }
}