#include "display_ui.h"
#include "../config.h"
#include <TFT_eSPI.h>

namespace {
  TFT_eSPI tft;

  struct UI {
    int16_t W,H, margin, radius, gap;
    int16_t cardX, cardY, cardW, cardH;
    int16_t headerY, dividerY, statusY;
    int16_t btnY, btnW, btnH;
  } ui;

  void pickFontPx(int px){
    if(px <= 12) tft.setTextFont(1);
    else if(px <= 20) tft.setTextFont(2);
    else if(px <= 36) tft.setTextFont(4);
    else if(px <= 60) tft.setTextFont(6);
    else tft.setTextFont(6);
  }
  void drawText(const String& s, int x, int y, uint16_t fg, uint16_t bg, int px, uint8_t datum){
    pickFontPx(px);
    tft.setTextDatum(datum);
    tft.setTextColor(fg, bg);
    tft.drawString(s, x, y);
  }

  void layout(){
    ui.W = tft.width();
    ui.H = tft.height();

    ui.margin = ui.W * 0.04;
    ui.radius = ui.W * 0.035;
    ui.gap    = ui.H * 0.012;

    ui.cardX = ui.margin;
    ui.cardY = ui.margin;
    ui.cardW = ui.W - 2*ui.margin;
    ui.cardH = ui.H - 2*ui.margin;

    ui.headerY  = ui.cardY + (ui.H * 0.06);
    ui.dividerY = ui.cardY + (ui.H * 0.20);
    ui.statusY  = ui.cardY + (ui.H * 0.26);

    ui.btnY   = ui.cardY + ui.cardH - (ui.H * 0.18);
    ui.btnW   = (ui.cardW - ui.margin) / 2;
    ui.btnH   = ui.H * 0.16;
  }

  void drawStatic() {
    layout();
    tft.fillScreen(COL_BG);
    tft.fillRoundRect(ui.cardX, ui.cardY, ui.cardW, ui.cardH, ui.radius, COL_CARD);

    // Header
    drawText("ProtoEtch", ui.cardX + ui.margin/2, ui.headerY, COL_SILVER, COL_CARD, ui.H*0.08, TL_DATUM);
    drawText("PF Design Labs", ui.cardX + ui.cardW - ui.margin/2, ui.headerY, COL_SILVER, COL_CARD, ui.H*0.06, TR_DATUM);

    // Divider
    tft.drawLine(ui.cardX + ui.margin/3, ui.dividerY, ui.cardX + ui.cardW - ui.margin/3, ui.dividerY, COL_SILVER);

    // Static labels
    const int step = ui.H * 0.12;
    drawText("TEMP:",   ui.cardX + ui.margin/2, ui.statusY + 0*step, COL_SILVER, COL_CARD, ui.H*0.07, TL_DATUM);
    drawText("TARGET:", ui.cardX + ui.margin/2, ui.statusY + 1*step, COL_SILVER, COL_CARD, ui.H*0.07, TL_DATUM);
    drawText("HEATER:", ui.cardX + ui.margin/2, ui.statusY + 2*step, COL_SILVER, COL_CARD, ui.H*0.07, TL_DATUM);

    // Buttons (decorative for now)
    const int r = ui.W * 0.03;
    int Lx = ui.cardX + ui.margin/2;
    int Rx = ui.cardX + ui.cardW - ui.btnW - ui.margin/2;
    tft.fillRoundRect(Lx, ui.btnY, ui.btnW, ui.btnH, r, COL_ORANGE);
    tft.fillRoundRect(Rx, ui.btnY, ui.btnW, ui.btnH, r, COL_ORANGE);
    drawText("MIX ETCHANT", Lx + ui.btnW/2, ui.btnY + ui.btnH/2, COL_WHITE, COL_ORANGE, ui.H*0.05, MC_DATUM);
    drawText("RUN ETCH",    Rx + ui.btnW/2, ui.btnY + ui.btnH/2, COL_WHITE, COL_ORANGE, ui.H*0.05, MC_DATUM);
  }

  // Helpers to clear dynamic fields cleanly
  void clearLineArea(int y, int h){
    tft.fillRect(ui.cardX + ui.margin/2 + 120, y - h, ui.cardW - ui.margin - 140, h + 6, COL_CARD);
  }
}

namespace DisplayUI {

void begin() {
  tft.init();
  tft.setRotation(1); // Landscape 320x240
#ifdef TFT_BL
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);
#endif
  drawStatic();
}

void update(float tempC, float setpointC, bool heaterOn) {
  const int step = ui.H * 0.12;
  const int lineH = ui.H * 0.08;

  // TEMP value
  {
    clearLineArea(ui.statusY + 0*step, lineH);
    char buf[32];
    if (!isnan(tempC)) snprintf(buf, sizeof(buf), "%.2f\xB0""C", tempC);
    else               snprintf(buf, sizeof(buf), "--.-\xB0""C");
    drawText(buf, ui.cardX + ui.margin/2 + 120, ui.statusY + 0*step, COL_WHITE, COL_CARD, ui.H*0.07, TL_DATUM);
  }

  // TARGET value
  {
    clearLineArea(ui.statusY + 1*step, lineH);
    char buf[32];
    snprintf(buf, sizeof(buf), "%.1f\xB0""C", setpointC);
    drawText(buf, ui.cardX + ui.margin/2 + 120, ui.statusY + 1*step, COL_WHITE, COL_CARD, ui.H*0.07, TL_DATUM);
  }

  // HEATER state
  {
    clearLineArea(ui.statusY + 2*step, lineH);
    drawText(heaterOn ? "ON" : "OFF",
             ui.cardX + ui.margin/2 + 120,
             ui.statusY + 2*step,
             heaterOn ? COL_ORANGE : COL_SILVER,
             COL_CARD, ui.H*0.07, TL_DATUM);
  }
}

} // namespace DisplayUI