#pragma once
#include <Arduino.h>

namespace DisplayUI {

/** Draw a splash screen with a fading ProtoEtch logo, then return. */
void splash();

/**
 * Initialize the TFT_eSPI display and draw all static UI chrome.
 * - Sets rotation to landscape (320x240 assumed via build flags).
 * - Clears background and draws header, section labels, and action area.
 * - Fonts are configured via TFT_eSPI FreeFonts (LOAD_GFXFF=1).
 */
void begin();

/**
 * Refresh the dynamic UI values without flicker.
 * The function only redraws fields whose values actually changed since
 * the last call (internal cache), minimizing overdraw and shimmer.
 *
 * Parameters
 * - tempC:          Current temperature in Celsius (NaN if invalid).
 * - setpointC:      Heater setpoint in Celsius.
 * - heaterOn:       Current heater state.
 * - agitateOn:      Agitation (pump) state.
 * - agitatePct:     Agitation power percentage [0..100].
 * - timeRemainingSec: Remaining etch time in seconds (renders as MM:SS).
 * - wifiOk/mqttOk:  Reserved for future status indicators (ignored today).
 */
void update(float tempC,
            float setpointC,
            bool  heaterOn,
            bool  agitateOn = false,
            uint32_t timeRemainingSec = 0,
            bool  wifiOk = false,
            bool  mqttOk = false);

} // namespace DisplayUI
