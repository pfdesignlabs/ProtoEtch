#pragma once
#include <Arduino.h>

/* ----------------- Lightweight logging ----------------- */
#ifndef PE_DEBUG
  #define PE_DEBUG 1
#endif
#if PE_DEBUG
  #define LOGI(...) do { Serial.printf(__VA_ARGS__); } while (0)
  #define LOGW(...) do { Serial.printf(__VA_ARGS__); } while (0)
  #define LOGE(...) do { Serial.printf(__VA_ARGS__); } while (0)
#else
  #define LOGI(...) do {} while (0)
  #define LOGW(...) do {} while (0)
  #define LOGE(...) do {} while (0)
#endif

/* ----------------- DS18B20 (Celsius) ----------------- */
// 1-Wire data pin + 4.7k pull-up to 3.3V
#ifndef TS_PIN
  #define TS_PIN 21
#endif
#ifndef TS_RES
  #define TS_RES 12     // 9..12 (12 ≈ 750ms conversion time)
#endif
#define TS_DEFAULT_PERIOD_MS  1000
#define TS_TIMEOUT_MS         1500

/* ----------------- Heater relay hardware ----------------- */
#ifndef PIN_HEATER_RELAY
  #define PIN_HEATER_RELAY 26
#endif
// Set to 1 if relay is ON with HIGH, 0 if ON with LOW
#ifndef HEATER_ACTIVE_HIGH
  #define HEATER_ACTIVE_HIGH 1
#endif
#define HEATER_RELAY_ON   (HEATER_ACTIVE_HIGH ? HIGH : LOW)
#define HEATER_RELAY_OFF  (HEATER_ACTIVE_HIGH ? LOW  : HIGH)

/* ----------------- Heater control defaults ----------------- */
#ifndef HEATER_SETPOINT_C
  #define HEATER_SETPOINT_C    45.0f
#endif
#ifndef HEATER_HYST_C
  #define HEATER_HYST_C         0.8f
#endif
#ifndef HEATER_MAX_TEMP_C
  #define HEATER_MAX_TEMP_C    60.0f
#endif
#ifndef HEATER_MIN_ON_MS
  #define HEATER_MIN_ON_MS     15000UL
#endif
#ifndef HEATER_MIN_OFF_MS
  #define HEATER_MIN_OFF_MS    15000UL
#endif

/* ----------------- Theme (GT40-ish) ----------------- */
static inline uint16_t rgb565(uint32_t hex) {
  uint8_t r=(hex>>16)&0xFF, g=(hex>>8)&0xFF, b=hex&0xFF;
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}
#define COL_BG      rgb565(0x1F3A5F) // RAL 5003-ish (Prussian blue)
#define COL_CARD    rgb565(0x0D2744)
#define COL_SILVER  rgb565(0xA5A5A5) // RAL 9006-ish
#define COL_ORANGE  rgb565(0xFF7F00)
#define COL_WHITE   rgb565(0xFFFFFF)