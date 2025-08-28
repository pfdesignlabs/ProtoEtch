#pragma once
#include <Arduino.h>

/* ---------- Logging (lightweight) ---------- */
#ifndef PE_DEBUG
  #define PE_DEBUG 1
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

/* ---------- DS18B20 configuration (Celsius) ---------- */
/* You may override via build flags:
 *   -DTS_PIN=21 -DTS_RES=12
 */
#ifndef TS_PIN
  #define TS_PIN 21          // DATA pin + 4.3–4.7k pull-up to 3.3V
#endif
#ifndef TS_RES
  #define TS_RES 12          // 9..12 (12 ≈ 750 ms conversion)
#endif

#define TS_DEFAULT_PERIOD_MS   1000    // sampling period (ms)
#define TS_TIMEOUT_MS          1500    // conversion timeout (ms)
#define TS_MAX_BACKOFF_POW     4       // up to 16× period on repeated errors

/* ---------- Heater relay hardware ---------- */
#define PIN_HEATER_RELAY   26
#define HEATER_RELAY_ON    HIGH        // your module is HIGH-trigger
#define HEATER_RELAY_OFF   LOW

/* ---------- Heater controller defaults (°C / ms) ---------- */
#define HEATER_SETPOINT_C     45.0f
#define HEATER_HYST_C          0.8f
#define HEATER_MAX_TEMP_C     60.0f
#define HEATER_MIN_ON_MS   15000UL
#define HEATER_MIN_OFF_MS  15000UL

/* ---------- NVS namespace/keys ---------- */
#define NVS_NS_PROTOETCH      "protoetch"
#define NVS_KEY_HT_SET        "ht_set"    // float (°C)
#define NVS_KEY_HT_HYST       "ht_hyst"   // float (°C)
#define NVS_KEY_HT_EN         "ht_en"     // uint8 0/1
#define NVS_KEY_HT_MAX        "ht_max"    // float (°C)
#define NVS_KEY_HT_MINON      "ht_mon"    // uint32 (ms)
#define NVS_KEY_HT_MINOFF     "ht_moff"   // uint32 (ms)