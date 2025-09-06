// Display UI implementation (TFT_eSPI)
//
// Responsibilities
// - One-time static chrome rendering (header, section labels, action area)
// - Flicker-free dynamic value updates (only redraw on change)
// - Consistent fonts and safe margins for readability
//
// Notes
// - Requires TFT_eSPI with FreeFonts enabled (LOAD_GFXFF=1).
// - The layout assumes 320x240 landscape (set via build flags).
// - All colors come from config.h theme constants.

#include "display_ui.h"
#include "../config.h"
#include <TFT_eSPI.h>
#include <math.h>
// FreeFonts are included automatically by TFT_eSPI when LOAD_GFXFF=1

namespace {
  TFT_eSPI tft;

  // Cached layout metrics and coordinates for drawing
  struct UI {
    int16_t W,H, margin, radius, gap;
    int16_t cardX, cardY, cardW, cardH;

    // Columns
    int16_t labelX, indentX, valueLeftX, valueX;

    // Rows
    int16_t titleY, dividerY; // top bar
    // Heater block
    int16_t heaterHdrY;
    int16_t heaterStateY;
    int16_t heaterTempsY; // current / goal
    // Agitation block
    int16_t agitHdrY;
    int16_t agitStateY;

    // Buttons
    int16_t btnY, btnW, btnH, btnX;

    // Time Remaining (now under Etch section)
    int16_t timeLabelY, timeValueY; // legacy (bottom timer) not used

    // Status (WiFi/MQTT)
    int16_t statusY; // in header bar
    int16_t statusRightX;

    // Metrics
    int16_t lineH, step, hdrPx, labelPx, valuePx, btnPx, timePx;
    int16_t labelPxBig, valuePxBig; // for State/Temp/Power lines

    // Etch section
    int16_t etchHdrY;
    int16_t etchTimeY;
  } ui;

  // Cache of last-drawn dynamic values to avoid unnecessary redraws (reduce flicker)
  struct Cache {
    bool     inited       = false;
    bool     heaterOn     = false;
    bool     agitOn       = false;
    bool     tempValid    = false;
    int      curTempI     = 0;   // rounded current temp (°C)
    int      setpointI    = 0;   // rounded setpoint (°C)
    uint32_t timeSec      = 0;   // remaining seconds
  } cache;

  // Legacy pixel-size mapper (kept for internal sizing; FreeFonts are used)
  void pickFontPx(int px){
    if(px <= 12) tft.setTextFont(1);
    else if(px <= 20) tft.setTextFont(2);
    else if(px <= 36) tft.setTextFont(4);
    else if(px <= 60) tft.setTextFont(6);
    else tft.setTextFont(6);
  }
  // Convenience wrapper to draw a string with datum and colors
  void drawText(const String& s, int x, int y, uint16_t fg, uint16_t bg, int /*px*/, uint8_t datum){
    tft.setTextDatum(datum);
    tft.setTextColor(fg, bg);
    tft.drawString(s, x, y);
  }

  // Font presets for a consistent look
  inline void useHeaderFont(){ tft.setFreeFont(&FreeSansBold9pt7b); }
  inline void useLabelFont() { tft.setFreeFont(&FreeSans9pt7b); }
  inline void useValueFont() { tft.setFreeFont(&FreeMonoBold9pt7b); }
  inline void useButtonFont(){ tft.setFreeFont(&FreeSansBold9pt7b); }

  // Compute screen-dependent geometry. Safe margins prevent bezel clipping.
  void layout(){
    ui.W = tft.width();
    ui.H = tft.height();

    ui.margin = ui.W * 0.025;  // shrink light border for more usable space
    ui.radius = ui.W * 0.030;  // slightly tighter corners
    ui.gap    = ui.H * 0.012;

    ui.cardX = ui.margin;
    ui.cardY = ui.margin;
    ui.cardW = ui.W - 2*ui.margin;
    ui.cardH = ui.H - 2*ui.margin;

    // Typography
    ui.hdrPx      = ui.H * 0.064; // section headers
    ui.labelPx    = ui.H * 0.047; // uniform, iets kleiner
    ui.valuePx    = ui.H * 0.047;
    ui.labelPxBig = ui.labelPx;   // gelijkgetrokken
    ui.valuePxBig = ui.valuePx;
    ui.btnPx      = ui.H * 0.043; // kleinere knoptekst
    ui.timePx     = ui.valuePx;   // timer gelijk aan values
    ui.lineH      = ui.H * 0.074;
    ui.step       = ui.H * 0.092; // row spacing

    // Columns
    ui.labelX     = ui.cardX + ui.margin/2;
    ui.indentX    = ui.labelX + (ui.W * 0.08);          // indentation under section headers
    ui.valueX     = ui.cardX + ui.cardW - ui.margin - (ui.W * 0.09); // larger safe-right margin
    ui.valueLeftX = ui.cardX + (ui.cardW * 0.52);       // left edge of value-clear area

    // Rows (sections)
    const int16_t top = ui.cardY + ui.margin; // inner top
    ui.titleY      = 2; // exactly 2px from top edge to top of text (TC_DATUM)
    // Divider exactly 2px below bottom of the header font
    useHeaderFont();
    int16_t hdrH = tft.fontHeight();
    ui.dividerY    = ui.titleY + hdrH + 2;
    ui.statusY     = ui.titleY; // align with title
    ui.statusRightX= ui.cardX + ui.cardW - ui.margin;
    
    // Etch rows
    ui.etchHdrY     = ui.dividerY + (ui.H * 0.05);
    ui.etchTimeY    = ui.etchHdrY + ui.step;

    // Heater rows
    ui.heaterHdrY   = ui.etchHdrY + 2*ui.step;
    ui.heaterStateY = ui.heaterHdrY + ui.step;
    ui.heaterTempsY = ui.heaterHdrY + 2*ui.step;
    // Agitation rows
    ui.agitHdrY     = ui.heaterHdrY + 3*ui.step;
    ui.agitStateY   = ui.agitHdrY + ui.step;

    // Button (single, centered) absolute afstand tot onderrand: onderkant 2px boven schermrand
    {
      ui.btnW   = (ui.cardW * 0.38);
      ui.btnH   = ui.H * 0.072 + 2; // +2 px taller
      // Add more bottom space: 8px margin from bottom
      ui.btnY   = ui.H - 8 - ui.btnH; // bottom = H-8
      ui.btnX   = ui.cardX + (ui.cardW - ui.btnW)/2;
      // legacy timer vars retained but unused for bottom timer
      ui.timeLabelY = ui.btnY + ui.btnH + (ui.gap * 0.4);
      ui.timeValueY = ui.btnY + ui.btnH + (ui.gap * 0.9);
    }
  }

  // Faux-bold by overdrawing with small offsets
  // Faux-bold helper (simple multi-pass draw)
  void drawTextBold(const String& s, int x, int y, uint16_t fg, uint16_t bg, int px, uint8_t datum){
    drawText(s, x,   y,   fg, bg, px, datum);
    drawText(s, x+1, y,   fg, bg, px, datum);
    drawText(s, x,   y+1, fg, bg, px, datum);
    drawText(s, x+1, y+1, fg, bg, px, datum);
  }

  void drawStatic() {
    layout();
    tft.fillScreen(COL_BG);
    // No inner card: draw directly on background for maximum usable space

    // Header title centered + divider
    useHeaderFont();
    drawTextBold("ProtoEtch", ui.cardX + ui.cardW/2, ui.titleY, COL_ORANGE, COL_BG, ui.hdrPx, TC_DATUM);
    tft.drawLine(ui.cardX + ui.margin/3, ui.dividerY, ui.cardX + ui.cardW - ui.margin/3, ui.dividerY, COL_SILVER);
    
    // Section headers + static labels
    useHeaderFont();
    drawTextBold("Etch", ui.labelX, ui.etchHdrY, COL_SILVER, COL_BG, ui.hdrPx, TL_DATUM);
    useLabelFont();
    drawText("Remaining:", ui.indentX, ui.etchTimeY, COL_SILVER, COL_BG, ui.labelPxBig, TL_DATUM);

    useHeaderFont();
    drawTextBold("Heater", ui.labelX, ui.heaterHdrY, COL_SILVER, COL_BG, ui.hdrPx, TL_DATUM);
    useLabelFont();
    drawText("State:", ui.indentX, ui.heaterStateY, COL_SILVER, COL_BG, ui.labelPxBig, TL_DATUM);
    drawText("Temp:",  ui.indentX, ui.heaterTempsY, COL_SILVER, COL_BG, ui.labelPxBig, TL_DATUM);
    
    useHeaderFont();
    drawTextBold("Agitation", ui.labelX, ui.agitHdrY, COL_SILVER, COL_BG, ui.hdrPx, TL_DATUM);
    useLabelFont();
    drawText("State:", ui.indentX, ui.agitStateY, COL_SILVER, COL_BG, ui.labelPxBig, TL_DATUM);

    // No divider above the action area (more breathing room for the button)

    // Buttons (fixed size from layout, precise vertical centering)
    const int r = ui.W * 0.03;
    tft.fillRoundRect(ui.btnX, ui.btnY, ui.btnW, ui.btnH, r, COL_ORANGE);
    useButtonFont();
    tft.setTextDatum(MC_DATUM); // middle-center for exact centering
    tft.setTextColor(COL_WHITE, COL_ORANGE);
    tft.drawString("Run Etch", ui.btnX + ui.btnW/2, ui.btnY + ui.btnH/2);
    // Ensure no stray 1px artefact remains below the button
    int yScrub = ui.btnY + ui.btnH;
    if (yScrub < ui.H) tft.fillRect(0, yScrub, ui.W, ui.H - yScrub, COL_BG);

    // No static settings label; a gear icon is displayed in header via update()
    // No "Time Remaining" label; only the timer value is rendered dynamically.
  }

  // Helpers to clear dynamic fields cleanly (right column)
  // Clear the area behind a right-aligned value line using the active font height
  void clearValueArea(int y){
    // Clear a tight band for the value line, based on current font height
    int16_t fh = tft.fontHeight();
    int16_t h  = fh + 4; // small padding
    int16_t w  = (ui.cardX + ui.cardW - ui.margin) - ui.valueLeftX; // to right edge inside margin
    int16_t y0 = y - 1;  // align with TR_DATUM (top-aligned values)
    if (w < 0) w = 0;
    tft.fillRect(ui.valueLeftX, y0, w, h, COL_BG);
  }
  // Clear the area behind a label line (left side)
  void clearRowArea(int y){
    int16_t w = ui.cardW - ui.margin;
    int16_t h = ui.lineH + 4;
    int16_t x = ui.labelX;
    int16_t y0 = y - 2; // small top padding
    tft.fillRect(x, y0, w, h, COL_BG);
  }

  // Simple status icons (WiFi bars, MQTT three dots)
  void drawWifiIcon(int x, int y, bool ok){
    uint16_t c = ok ? COL_ORANGE : COL_SILVER;
    int bw = ui.W * 0.01; if (bw < 2) bw = 2;
    int bh1 = ui.H * 0.014; int bh2 = ui.H * 0.021; int bh3 = ui.H * 0.028;
    int gap = bw;
    // draw from baseline up, bottoms aligned at y
    tft.fillRect(x,              y - bh1, bw, bh1, c);
    tft.fillRect(x + (gap + bw), y - bh2, bw, bh2, c);
    tft.fillRect(x + 2*(gap + bw), y - bh3, bw, bh3, c);
  }
  void drawMqttIcon(int x, int y, bool ok){
    uint16_t c = ok ? COL_ORANGE : COL_SILVER;
    int r = ui.H * 0.008; if (r < 2) r = 2;
    int gap = r*2 + 2;
    tft.drawLine(x-r, y, x+2*gap+r, y, c);
    tft.fillCircle(x,          y, r, c);
    tft.fillCircle(x+gap,      y, r, c);
    tft.fillCircle(x+2*gap,    y, r, c);
  }
  void drawSettingsIcon(int x, int y){
    // Simple gear: ring + 6 teeth
    uint16_t c = COL_SILVER;
    int R = ui.H * 0.018; if (R < 5) R = 5;
    int r = R - 3; if (r < 2) r = 2;
    tft.drawCircle(x, y, R, c);
    tft.drawCircle(x, y, r, c);
    for (int k=0; k<6; ++k){
      float a = k * 3.14159f/3.0f; // 60 deg
      int x1 = x + (int)(r * cos(a));
      int y1 = y + (int)(r * sin(a));
      int x2 = x + (int)((R+1) * cos(a));
      int y2 = y + (int)((R+1) * sin(a));
      tft.drawLine(x1, y1, x2, y2, c);
    }
  }
  void clearTimeArea(){
    int16_t w = ui.cardW - ui.margin;
    int16_t h = ui.lineH + 8;
    int16_t x = ui.cardX + (ui.margin/2);
    int16_t y = ui.timeValueY - ui.lineH;
    tft.fillRect(x, y, w, h, COL_BG);
  }
}

namespace DisplayUI {

void splash() {
  // Black background splash with a fading logo/text
  tft.fillScreen(TFT_BLACK);

  // Compute positions
  const int cx = tft.width() / 2;
  const int cy = tft.height() / 2;

  // Helper: simple geometric logo at left of text
  auto drawMiniLogo = [&](uint16_t col){
    // Stylized traces (approximation of provided mark)
    int x0 = cx - 70; int y0 = cy - 28;
    // Vertical bar
    tft.fillRoundRect(x0, y0 - 20, 10, 60, 3, col);
    // Three descending traces with pads
    tft.drawLine(x0 + 18, y0 + 0,  x0 + 40, y0 + 0,  col);
    tft.drawLine(x0 + 18, y0 + 14, x0 + 40, y0 + 14, col);
    tft.drawLine(x0 + 18, y0 + 28, x0 + 40, y0 + 28, col);
    tft.fillCircle(x0 + 44, y0 + 0,  3, col);
    tft.fillCircle(x0 + 44, y0 + 14, 3, col);
    tft.fillCircle(x0 + 44, y0 + 28, 3, col);
  };

  // Fade steps from dark gray to white (slower, smoother)
  useHeaderFont();
  const char* title = "PROTOETCH";
  const int steps = 40;      // total fade steps
  const int stepDelay = 40;  // ms per step
  for (int step = 0; step <= steps; ++step) {
    uint8_t v = (uint8_t)((step * 255) / steps);
    uint16_t col = tft.color565(v, v, v);
    // Clear frame
    tft.fillScreen(TFT_BLACK);
    // Draw logo and title
    drawMiniLogo(col);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(col, TFT_BLACK);
    tft.drawString(title, cx + 30, cy);
    delay(stepDelay);
  }
  // Hold splash for 20 seconds before proceeding to main UI
  delay(20000);
}

void begin() {
  tft.init();
  tft.setRotation(1); // Landscape 320x240
#ifdef TFT_BL
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);
#endif
  tft.setTextWrap(false);
  // Splash first, then the main chrome
  splash();
  drawStatic();
}

void update(float tempC,
            float setpointC,
            bool  heaterOn,
            bool  agitateOn,
            uint32_t timeRemainingSec,
            bool  wifiOk,
            bool  mqttOk) {

  // Header status icons removed for now (space reserved for future use)

  // Heater block
  {
    useValueFont();
    // State line – redraw only if changed
    if (!cache.inited || cache.heaterOn != heaterOn) {
      clearValueArea(ui.heaterStateY);
      drawText(heaterOn ? "ON" : "OFF",
               ui.valueX, ui.heaterStateY,
               heaterOn ? COL_ORANGE : COL_SILVER,
               COL_BG, ui.valuePxBig, TR_DATUM);
      cache.heaterOn = heaterOn;
    }

    // Temp line (cur/goal on right) – compare integer rounding and validity
    bool valid = !isnan(tempC);
    int curI   = valid ? (int)lrintf(tempC) : 0;
    int spI    = (int)lrintf(setpointC);
    if (!cache.inited || cache.tempValid != valid || cache.curTempI != curI || cache.setpointI != spI) {
      clearValueArea(ui.heaterTempsY);
      char buf[32];
      if (valid) snprintf(buf, sizeof(buf), "%dC / %dC", curI, spI);
      else       snprintf(buf, sizeof(buf), "--C / %dC", spI);
      drawText(buf, ui.valueX, ui.heaterTempsY, COL_WHITE, COL_BG, ui.valuePxBig, TR_DATUM);
      cache.tempValid = valid;
      cache.curTempI  = curI;
      cache.setpointI = spI;
    }
  }

  // Agitation block (static power, only state displayed)
  {
    useValueFont();
    // State line – redraw only if changed
    if (!cache.inited || cache.agitOn != agitateOn) {
      clearValueArea(ui.agitStateY);
      drawText(agitateOn ? "ON" : "OFF",
               ui.valueX, ui.agitStateY,
               agitateOn ? COL_ORANGE : COL_SILVER,
               COL_BG, ui.valuePxBig, TR_DATUM);
      cache.agitOn = agitateOn;
    }
  }

  // Etch time (MM:SS) displayed in the Etch section
  {
    useValueFont();
    uint32_t sec = timeRemainingSec;
    if (!cache.inited || cache.timeSec != sec) {
      clearValueArea(ui.etchTimeY);
      uint32_t mm = sec / 60; uint32_t ss = sec % 60;
      char buf[32];
      snprintf(buf, sizeof(buf), "%02u:%02u", (unsigned)mm, (unsigned)ss);
      drawText(buf, ui.valueX, ui.etchTimeY, COL_WHITE, COL_BG, ui.timePx, TR_DATUM);
      cache.timeSec = sec;
    }
  }
  cache.inited = true;
}

} // namespace DisplayUI
