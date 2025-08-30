#include "display_ui.h"

// ---- GT40 theme colors ----
static inline uint16_t rgb565(uint32_t hex){
  uint8_t r=(hex>>16)&0xFF, g=(hex>>8)&0xFF, b=hex&0xFF;
  return ((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3);
}
static const uint16_t COL_BG     = rgb565(0x1F3A5F); // RAL 5003-ish
static const uint16_t COL_CARD   = rgb565(0x0D2744);
static const uint16_t COL_SILVER = rgb565(0xA5A5A5); // RAL 9006-ish
static const uint16_t COL_ORANGE = rgb565(0xFF7F00);
static const uint16_t COL_WHITE  = rgb565(0xFFFFFF);

static TFT_eSPI tft;

// cache last drawn state to avoid full redraws too often (optional)
static UIStatus last{};
static bool firstPaint = true;

static void pickFontPx(int px){
  if(px <= 12) tft.setTextFont(1);
  else if(px <= 20) tft.setTextFont(2);
  else if(px <= 36) tft.setTextFont(4);
  else if(px <= 60) tft.setTextFont(6);
  else tft.setTextFont(8);
}
static void drawText(const String& s, int x, int y, uint16_t fg, uint16_t bg, int px, uint8_t datum){
  pickFontPx(px);
  tft.setTextDatum(datum);
  tft.setTextColor(fg, bg);
  tft.drawString(s, x, y);
}

namespace {
  struct UI {
    int16_t W,H, margin,radius,gap;
    int16_t cardX,cardY,cardW,cardH;
    int16_t headerY,dividerY,statusY;
    int16_t btnY,btnW,btnH;
  } ui;

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
    ui.btnY     = ui.cardY + ui.cardH - (ui.H * 0.18);
    ui.btnW     = (ui.cardW - ui.margin) / 2;
    ui.btnH     = ui.H * 0.16;
  }
}

void DisplayUI::begin(){
  tft.init();
  tft.setRotation(1); // landscape 320x240
  tft.fillScreen(COL_BG);
  firstPaint = true;
}

void DisplayUI::render(const UIStatus& st){
  layout();

  if (firstPaint){
    tft.fillScreen(COL_BG);
    tft.fillRoundRect(ui.cardX, ui.cardY, ui.cardW, ui.cardH, ui.radius, COL_CARD);

    // header
    drawText("ProtoEtch", ui.cardX + ui.margin/2, ui.headerY, COL_SILVER, COL_CARD, ui.H*0.08, TL_DATUM);
    drawText(String("WiFi: ") + (st.wifiOk ? "OK" : "OFF"),
             ui.cardX + ui.cardW - ui.margin/2, ui.headerY, COL_SILVER, COL_CARD, ui.H*0.07, TR_DATUM);

    // divider
    tft.drawLine(ui.cardX + ui.margin/3, ui.dividerY, ui.cardX + ui.cardW - ui.margin/3, ui.dividerY, COL_SILVER);

    // static buttons
    int btnRadius = ui.W * 0.03;
    int Lx = ui.cardX + ui.margin/2;
    int Rx = ui.cardX + ui.cardW - ui.btnW - ui.margin/2;
    tft.fillRoundRect(Lx, ui.btnY, ui.btnW, ui.btnH, btnRadius, COL_ORANGE);
    tft.fillRoundRect(Rx, ui.btnY, ui.btnW, ui.btnH, btnRadius, COL_ORANGE);
    drawText("MIX ETCHANT", Lx + ui.btnW/2, ui.btnY + ui.btnH/2, COL_WHITE, COL_ORANGE, ui.H*0.05, MC_DATUM);
    drawText("RUN ETCH",    Rx + ui.btnW/2, ui.btnY + ui.btnH/2, COL_WHITE, COL_ORANGE, ui.H*0.05, MC_DATUM);

    // footer
    drawText("PF Design Labs", ui.cardX + ui.cardW/2, ui.cardY + ui.cardH - ui.margin/2, COL_SILVER, COL_CARD, ui.H*0.07, MC_DATUM);

    firstPaint = false;
  }

  // dynamic status text region (overpaint with card color then redraw)
  int lineStep = ui.H * 0.12;
  int x = ui.cardX + ui.margin/2;
  int y0 = ui.statusY;

  // clear area (simple: draw filled rect over the block)
  int clearW = ui.cardW - ui.margin;
  int clearH = lineStep * 3 + 6;
  tft.fillRect(x, y0 - (ui.H*0.08)/2, clearW, clearH, COL_CARD);

  // redraw
  String tempTxt = "TEMP: ";
  if (isnan(st.tempC)) tempTxt += "--.- \xB0""C";
  else tempTxt += String(st.tempC, 1) + " \xB0""C";
  drawText(tempTxt, x, y0 + 0*lineStep, COL_SILVER, COL_CARD, ui.H*0.07, TL_DATUM);
  drawText(String("HEATER: ") + (st.heaterOn ? "ON" : "OFF"), x, y0 + 1*lineStep, COL_SILVER, COL_CARD, ui.H*0.07, TL_DATUM);
  drawText(String("PUMP: ")   + (st.pumpOn ? "ON" : "OFF"),   x, y0 + 2*lineStep, COL_SILVER, COL_CARD, ui.H*0.07, TL_DATUM);

  last = st;
}