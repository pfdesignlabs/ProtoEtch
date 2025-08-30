#pragma once
#include <Arduino.h>

/* -------- Logging -------- */
#ifndef PE_DEBUG
  #define PE_DEBUG 1
#endif
#if PE_DEBUG
  #define LOGI(...)  do { Serial.printf(__VA_ARGS__); } while (0)
  #define LOGW(...)  do { Serial.printf(__VA_ARGS__); } while (0)
  #define LOGE(...)  do { Serial.printf(__VA_ARGS__); } while (0)
#else
  #define LOGI(...)  do {} while (0)
  #define LOGW(...)  do {} while (0)
  #define LOGE(...)  do {} while (0)
#endif

/* -------- DS18B20 (Celsius) -------- */
#ifndef TS_PIN
  #define TS_PIN 21          // GPIO for 1-Wire data (4.7k pull-up to 3.3V)
#endif
#ifndef TS_RES
  #define TS_RES 12          // 9..12 (12 ≈ 750ms conversion)
#endif
#define TS_DEFAULT_PERIOD_MS 1000
#define TS_TIMEOUT_MS        1500

/* -------- Heater relay (not strictly needed for this build) -------- */
#define PIN_HEATER_RELAY 26
#define HEATER_RELAY_ON  HIGH   // change to LOW if your relay is active-low
#define HEATER_RELAY_OFF LOW