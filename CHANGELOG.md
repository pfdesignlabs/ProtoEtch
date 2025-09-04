---
title: Changelog
tags: [protoetch, changelog, versioning]
updated: 2025-08-28
---

# 📓 ProtoEtch Changelog

## [0.3.0] – 2025-09-03

### Added
- New TFT UI (feature/tft-display):
  - Header centered with exact 2 px top margin and 2 px spacing to divider.
  - Sections: Etch (Time), Heater (State, Temp), Agitation (State, Power).
  - Run Etch button with precise margins and centered label.
- FreeFonts integration (TFT_eSPI LOAD_GFXFF=1):
  - Headers: FreeSansBold 9pt, labels: FreeSans 9pt, values: FreeMonoBold 9pt.

### Changed
- Flicker-free updates: dynamic fields redraw only on value change (cached).
- Clear routines use actual font heights to avoid overdraw artifacts.
- Removed inner “card” panel; draw on background for more usable space.
- Simplified header status icons (temporarily removed). Agitation "Power" UI removed (agitation is static).

### Fixed
- Right-edge clipping of values by adding safer right margins.
- Button text centering (horizontal + vertical) and stray 1 px line under button.

### Developer
- Added docstrings and inline comments to Display UI code.

## [0.2.0] – 2025-08-28

### Added
- **Heater HAL (`heater.h/.cpp`)** to safely switch relay ON/OFF.
- **Heater Controller (`heater_controller.h/.cpp`)** with:
  - Bang-bang control with hysteresis.
  - Minimum on/off hold times (relay protection).
  - Failsafes: sensor stale, overtemp cutoff.
  - Remaining hold times in status struct.
  - NVS persistence for setpoint, hysteresis, enabled flag, cutoff, min on/off.
  - Input clamping to safe ranges.
  - Serial CLI for runtime tuning (`HEAT EN/SET/HYS/SAVE/LOAD/MINS/MAX/STATUS`).
- Standardized all temperatures in **Celsius** across codebase.

### Changed
- All comments translated to English.
- Updated `config.h` with clear logging macros, heater defaults, and NVS keys.

### Removed
- Mixed Fahrenheit references in comments/code.

---

## [0.1.0] – 2025-08-28

### Added
- **DS18B20 driver module (`sensor_ds18b20.h/.cpp`)**
  - Non-blocking state machine (no `delay()`).
  - Event-driven interface with `subscribe()` for callbacks.
  - Caching via `latest()` for fast, non-blocking reads.
  - Exponential moving average (EMA) smoothing configurable.
  - Error & stats reporting:
    - Timeout, CRC fail, out-of-range detection.
    - `lastError()` and `stats()` API.
  - CRC check on scratchpad for reliable measurements.
  - `healthy()` helper for fail-safe usage in other modules.
  - `forceSample()` to trigger an immediate conversion.
  - Exponential backoff on repeated failures.
  - Calibration API:
    - `setCalibration(gain, offset)`
    - Persistence via NVS: `loadCalibration()` and `saveCalibration()`.

### Changed
- **`config.h`**: added lightweight logging macros (`LOGI/LOGW/LOGE`) for debug output.
- Improved error handling when sensor is absent or unreachable.

### Removed
- Blocking wait during conversions.
- Raw, unprotected reading without CRC checks.
