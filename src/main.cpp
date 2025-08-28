#include <Arduino.h>
#include "config.h"
#include "sensor_ds18b20.h"
#include "heater.h"
#include "heater_controller.h"

static const char* reasonText(HeaterCtl::Reason r){
  switch(r){
    case HeaterCtl::Reason::Idle:           return "Idle";
    case HeaterCtl::Reason::BelowBand:      return "BelowBand";
    case HeaterCtl::Reason::AboveBand:      return "AboveBand";
    case HeaterCtl::Reason::MinOnHold:      return "MinOnHold";
    case HeaterCtl::Reason::MinOffHold:     return "MinOffHold";
    case HeaterCtl::Reason::SensorStale:    return "SensorStale";
    case HeaterCtl::Reason::OverTempCutoff: return "OverTempCutoff";
    case HeaterCtl::Reason::Disabled:       return "Disabled";
  }
  return "?";
}

void setup() {
  Serial.begin(115200);
  delay(150);
  LOGI("\n[Boot] ProtoEtch heater demo (°C everywhere)\n");

  // Sensor
  TempSensor::begin();
  TempSensor::setPeriod(1000);
  TempSensor::setResolution(12);
  TempSensor::setEMA(0.2f);

  // Heater controller
  HeaterCtl::begin();
  // Optional tuning while testing:
  // HeaterCtl::setMinOnOff(2000, 2000);
  // HeaterCtl::setHysteresis(1.0f);
  // HeaterCtl::setSetpoint(25.0f);
}

void loop() {
  TempSensor::tick();
  HeaterCtl::tick();
  HeaterCtl::serialCLI();       // type commands into Serial Monitor

  // Periodic log
  static uint32_t t=0;
  if (millis()-t > 2000) {
    t = millis();
    auto s = HeaterCtl::getStatus();
    Serial.printf("[Heater] T=%.2f C  set=%.1f C  on=%d  reason=%s  remOn=%u ms  remOff=%u ms\n",
      s.currentC, s.setpointC, s.relayOn, reasonText(s.reason), s.remainMinOnMs, s.remainMinOffMs);
  }
}