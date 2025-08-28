#include <Arduino.h>
#include "config.h"
#include "sensor_ds18b20.h"

static void onTemp(const TempSensor::Sample& s) {
  if (s.valid) {
    LOGI("[Temp] %.2f C @ %lu ms\n", s.c, s.ts);
  } else {
    LOGW("[Temp] invalid/timeout (err=%d)\n", (int)TempSensor::lastError());
  }
}

void setup() {
  Serial.begin(115200);
  delay(150);
  LOGI("\n[Boot] DS18B20 demo\n");

  TempSensor::begin();
  TempSensor::setPeriod(1000);     // 1 Hz
  TempSensor::setResolution(12);   // nauwkeurig
  TempSensor::setEMA(0.2f);        // zachte smoothing
  TempSensor::subscribe(onTemp);   // event-driven

  // Voorbeeld: eenmalige kalibratie laden (al in begin() gedaan), of overschrijven:
  // TempSensor::setCalibration(1.0f, -0.25f);  // offset-correctie
  // TempSensor::saveCalibration(1.0f, -0.25f); // persist
}

void loop() {
  TempSensor::tick();

  // Pollen is ook ok naast callbacks:
  static uint32_t t=0;
  if (millis() - t >= 2000) {
    t = millis();
    auto s = TempSensor::latest();
    if (s.valid) {
      LOGI("[Poll] %.2f C\n", s.c);
    } else {
      LOGW("[Poll] invalid (healthy=%d)\n", TempSensor::healthy());
    }
  }
}