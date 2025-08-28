#pragma once
#include <Arduino.h>

// ---------- Logging (lichtgewicht) ----------
#ifndef PE_DEBUG
  #define PE_DEBUG 1   // zet 0 om alle logs uit te zetten
#endif

#if PE_DEBUG
  #define LOGI(...)  do { Serial.printf(__VA_ARGS__); } while(0)
  #define LOGW(...)  do { Serial.printf(__VA_ARGS__); } while(0)
  #define LOGE(...)  do { Serial.printf(__VA_ARGS__); } while(0)
#else
  #define LOGI(...)  do {} while(0)
  #define LOGW(...)  do {} while(0)
  #define LOGE(...)  do {} while(0)
#endif

// ---------- DS18B20 configuratie ----------
// Je kunt deze ook via build_flags in platformio.ini zetten:
//   -DTS_PIN=21 -DTS_RES=12
#ifndef TS_PIN
  #define TS_PIN 21          // DATA pin + 4.3–4.7k pull-up naar 3.3V
#endif
#ifndef TS_RES
  #define TS_RES 12          // 9..12 (12 = ~750ms conversie)
#endif

// Defaults voor de sensor module
#define TS_DEFAULT_PERIOD_MS   1000     // 1 Hz sampling
#define TS_TIMEOUT_MS          1500     // safety timeout
#define TS_MAX_BACKOFF_POW     4        // max 16x period bij fout