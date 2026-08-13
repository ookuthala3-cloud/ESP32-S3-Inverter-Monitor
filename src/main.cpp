#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

// =====================================================
// TEST DATA — sensors will replace these later
// =====================================================
float acVoltage     = 230.4;
float acCurrent     = 2.42;
float acPower       = 557.0;
float frequencyHz   = 50.0;

float batteryVolt   = 12.6;
int   batteryPct    = 82;
float batteryMinV   = 12.1;
float batteryMaxV   = 13.8;
float batteryAvgV   = 12.5;
float batteryCurrent = 5.6;

float inverterTemp  = 38.0;

// =====================================================
// PAGE CONTROL
// =====================================================
uint8_t currentPage = 0;
const uint8_t PAGE_COUNT = 2;
const unsigned long PAGE_INTERVAL = 15000UL;
unsigned long lastPageChange = 0;

// =====================================================
// COLORS
// =====================================================
#define COL_BG        TFT_BLACK
#define COL_PANEL     0x1082
#define COL_BORDER    0x4208
#define COL_TEXT      TFT_WHITE
#define COL_DIM       0xBDF7

#define COL_YELLOW    TFT_YELLOW
#define COL_GREEN     TFT_GREEN
#define COL_BLUE      TFT_BLUE
#define COL_ORANGE    0xFD20
#define COL_RED       TFT_RED
#define COL_CYAN      TFT_CYAN

// =====================================================
// Helpers
// =====================================================
void drawCenteredText(const String& text, int x, int y, int font,
  uint16_t color, uint8_t datum) {
  tft.setTextDatum(datum);
  tft.setTextFont(font);
  tft.setTextColor(color);
  tft.drawString(text, x, y);
}

// =====================================================
// Gauge
// =====================================================
void drawGauge(
  int cx,
  int cy,
  int radius,
  float value,
  float minValue,
  float maxValue,
  uint16_t gaugeColor,
  String unit,
  unsigned int decimals
) {
  // Smooth upper gauge: left-lower -> top -> right-lower.
  // TFT_eSPI drawArc: 0 deg = top, angles increase clockwise.
  const int startAngle = 315;   // left-lower side
  const int sweepAngle = 270;   // long arc across the TOP, gap at bottom
  const int thickness = (radius >= 40) ? 8 : 5;

  float normalized =
      constrain((value - minValue) / (maxValue - minValue), 0.0f, 1.0f);
  int activeSweep = (int)round(normalized * sweepAngle);

  auto arcPiece = [&](int fromAngle, int toAngle, uint16_t color) {
    if (toAngle <= fromAngle) return;

    while (fromAngle >= 360) {
      fromAngle -= 360;
      toAngle -= 360;
    }

    if (toAngle <= 360) {
      tft.drawArc(cx, cy, radius, radius - thickness,
                  fromAngle, toAngle, color, COL_BG, true);
    } else {
      tft.drawArc(cx, cy, radius, radius - thickness,
                  fromAngle, 360, color, COL_BG, true);
      tft.drawArc(cx, cy, radius, radius - thickness,
                  0, toAngle - 360, color, COL_BG, true);
    }
  };

  // Background arc
  arcPiece(startAngle, startAngle + sweepAngle, COL_BORDER);

  // Active/value arc
  if (activeSweep > 0) {
    arcPiece(startAngle, startAngle + activeSweep, gaugeColor);
  }

  // Center value
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(COL_TEXT, COL_BG);
  tft.setTextSize(1);
  tft.drawString(String(value, decimals), cx, cy - 2, 2);

  tft.setTextColor(COL_DIM, COL_BG);
  tft.drawString(unit, cx, cy + 15, 1);

  tft.setTextDatum(TL_DATUM);
}

// =====================================================
// Battery icon
// =====================================================
void drawBattery(int x, int y, int percentage) {

  const int w = 38;
  const int h = 20;

  tft.drawRoundRect(x, y, w, h, 3, COL_GREEN);
  tft.fillRect(x + w, y + 6, 4, 8, COL_GREEN);

  int fillWidth = map(
    constrain(percentage, 0, 100),
    0,
    100,
    0,
    w - 6
  );

  tft.fillRect(
    x + 3,
    y + 3,
    fillWidth,
    h - 6,
    COL_GREEN
  );
}

// =====================================================
// Wi-Fi symbol (UI only for now)
// =====================================================
void drawWifi(int x, int y) {

  tft.drawCircle(x, y + 9, 2, TFT_WHITE);

  for (int r = 5; r <= 11; r += 3) {
    for (int a = 220; a <= 320; a += 5) {

      float rad = a * DEG_TO_RAD;

      int px = x + cos(rad) * r;
      int py = y + 9 + sin(rad) * r;

      tft.drawPixel(px, py, TFT_WHITE);
    }
  }
}

// =====================================================
// PAGE 1
// =====================================================
void drawMainPage() {

  tft.fillScreen(COL_BG);

  // ---------------------------------------------------
  // Top bar
  // ---------------------------------------------------
  tft.drawFastHLine(5, 29, 230, COL_BORDER);

  tft.setTextColor(COL_TEXT, COL_BG);
  tft.setTextSize(1);

  // Temporary date/time
  tft.setCursor(8, 10);
  tft.print("13 AUG 2026");

  tft.setCursor(150, 10);
  tft.print("08:00");

  drawWifi(222, 7);

  // ---------------------------------------------------
  // AC OUTPUT title
  // ---------------------------------------------------
  drawCenteredText(
    "AC OUTPUT",
    0,
    38,
    240,
    COL_YELLOW,
    1
  );

  // ---------------------------------------------------
  // Three gauges
  // ---------------------------------------------------
  drawGauge(
    43, 90,
    31,
    acVoltage,
    180,
    260,
    COL_GREEN,
    "V",
    1
  );

  drawGauge(
    120, 90,
    31,
    acCurrent,
    0,
    10,
    COL_BLUE,
    "A",
    2
  );

  drawGauge(
    197, 90,
    31,
    acPower,
    0,
    1000,
    COL_ORANGE,
    "W",
    0
  );

  // ---------------------------------------------------
  // Frequency
  // ---------------------------------------------------
  tft.drawFastHLine(8, 128, 224, COL_BORDER);

  tft.setTextColor(COL_CYAN, COL_BG);
  tft.setTextSize(2);

  tft.setCursor(20, 137);
  tft.print(frequencyHz, 1);

  tft.setTextSize(1);
  tft.print(" Hz");

  // status
  tft.setTextColor(COL_DIM, COL_BG);
  tft.setCursor(150, 140);
  tft.print("AC OK");

  // ---------------------------------------------------
  // Battery section
  // ---------------------------------------------------
  tft.drawFastHLine(8, 160, 224, COL_BORDER);

  drawBattery(15, 171, batteryPct);

  tft.setTextColor(COL_TEXT, COL_BG);
  tft.setTextSize(1);

  tft.setCursor(68, 169);
  tft.print(batteryVolt, 1);
  tft.print(" V");

  tft.setTextSize(2);
  tft.setCursor(68, 183);
  tft.print(batteryPct);
  tft.print("%");

  // ---------------------------------------------------
  // System status
  // ---------------------------------------------------
  tft.setTextSize(1);
  tft.setTextColor(COL_DIM, COL_BG);

  tft.setCursor(153, 169);
  tft.print("SYSTEM");

  tft.setTextColor(COL_GREEN, COL_BG);
  tft.setTextSize(1);

  tft.setCursor(153, 184);
  tft.print("NORMAL");

  tft.fillCircle(218, 183, 10, COL_GREEN);

  tft.setTextColor(TFT_BLACK, COL_GREEN);
  tft.setTextSize(1);
  tft.setCursor(215, 179);
  tft.print("V");

  // ---------------------------------------------------
  // Temperature footer
  // ---------------------------------------------------
  tft.drawFastHLine(8, 207, 224, COL_BORDER);

  tft.setTextColor(COL_DIM, COL_BG);
  tft.setTextSize(1);

  tft.setCursor(15, 220);
  tft.print("Inverter Temp:");

  tft.setTextColor(COL_TEXT, COL_BG);

  tft.setCursor(115, 220);
  tft.print(inverterTemp, 1);
  tft.print(" C");
}


// =====================================================
// PAGE 2 - BATTERY & DC
// =====================================================
void drawBatteryPage() {
  tft.fillScreen(COL_BG);

  // Header
  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(COL_GREEN, COL_BG);
  tft.drawString("BATTERY & DC", 120, 12);
  tft.drawFastHLine(8, 28, 224, COL_BORDER);

  // -----------------------------------------------------
  // Battery gauge - smooth upper semicircle (reference style)
  // -----------------------------------------------------
  const int cx = 120;
  const int cy = 91;
  const int outerR = 52;
  const int thickness = 9;

  // TFT_eSPI: 0 = top, clockwise.
  // 270 -> 360 -> 90 gives left -> top -> right.
  const int startA = 270;
  const int totalSweep = 180;
  int activeSweep = (int)round(constrain(batteryPct / 100.0f, 0.0f, 1.0f) * totalSweep);

  auto arcPiece = [&](int fromAngle, int toAngle, uint16_t color) {
    if (toAngle <= fromAngle) return;
    while (fromAngle >= 360) {
      fromAngle -= 360;
      toAngle -= 360;
    }
    if (toAngle <= 360) {
      tft.drawArc(cx, cy, outerR, outerR - thickness,
                  fromAngle, toAngle, color, COL_BG, true);
    } else {
      tft.drawArc(cx, cy, outerR, outerR - thickness,
                  fromAngle, 360, color, COL_BG, true);
      tft.drawArc(cx, cy, outerR, outerR - thickness,
                  0, toAngle - 360, color, COL_BG, true);
    }
  };

  // Grey full semicircle then green active portion
  arcPiece(startA, startA + totalSweep, COL_BORDER);
  if (activeSweep > 0) {
    arcPiece(startA, startA + activeSweep, COL_GREEN);
  }

  // Gauge center text with safe spacing
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(COL_TEXT, COL_BG);
  tft.setTextFont(4);
  tft.drawString(String(batteryPct), 112, 77);

  tft.setTextFont(2);
  tft.drawString("%", 148, 80);
  tft.drawString(String(batteryVolt, 1) + " V", 120, 104);

  // -----------------------------------------------------
  // MIN / MAX / AVG cards
  // -----------------------------------------------------
  const int cardY = 121;
  const int cardW = 70;
  const int cardH = 35;
  const int cardX[3] = {8, 85, 162};
  const char* labels[3] = {"MIN", "MAX", "AVG"};
  float vals[3] = {batteryMinV, batteryMaxV, batteryAvgV};

  for (int i = 0; i < 3; i++) {
    tft.drawRoundRect(cardX[i], cardY, cardW, cardH, 4, COL_BORDER);

    tft.setTextDatum(MC_DATUM);
    tft.setTextFont(1);
    tft.setTextColor(COL_DIM, COL_BG);
    tft.drawString(labels[i], cardX[i] + cardW / 2, cardY + 9);

    tft.setTextFont(2);
    tft.setTextColor(COL_TEXT, COL_BG);
    tft.drawString(String(vals[i], 1) + "V",
                   cardX[i] + cardW / 2, cardY + 24);
  }

  // -----------------------------------------------------
  // Discharge current
  // -----------------------------------------------------
  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(1);
  tft.setTextColor(COL_DIM, COL_BG);
  tft.drawString("DISCHARGE CURRENT", 10, 163);

  tft.setTextDatum(TR_DATUM);
  tft.setTextColor(COL_TEXT, COL_BG);
  tft.drawString(String(batteryCurrent, 1) + " A", 230, 163);

  const int barX = 10;
  const int barY = 176;
  const int segW = 18;
  const int segH = 8;
  const int gap = 4;
  const int barSegments = 10;

  int active = constrain(
    (int)round((batteryCurrent / 10.0f) * barSegments),
    0,
    barSegments
  );

  for (int i = 0; i < barSegments; i++) {
    uint16_t c = (i < active) ? COL_GREEN : COL_BORDER;
    tft.fillRoundRect(
      barX + i * (segW + gap),
      barY,
      segW,
      segH,
      2,
      c
    );
  }

  // -----------------------------------------------------
  // Footer/status - all safely inside 240x240
  // -----------------------------------------------------
  tft.drawFastHLine(8, 193, 224, COL_BORDER);

  drawBattery(12, 204, batteryPct);

  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(1);
  tft.setTextColor(COL_DIM, COL_BG);
  tft.drawString("STATUS", 67, 209);

  tft.setTextColor(COL_GREEN, COL_BG);
  tft.drawString("NORMAL", 111, 209);

  tft.fillCircle(218, 213, 7, COL_GREEN);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_BLACK, COL_GREEN);
  tft.drawString("OK", 218, 213, 1);

  tft.setTextDatum(TL_DATUM);
}

// =====================================================
// PAGE RENDERER
// =====================================================
void drawCurrentPage() {
  if (currentPage == 0) {
    drawMainPage();
  } else {
    drawBatteryPage();
  }
}

// =====================================================
// SETUP
// =====================================================
void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("============================");
  Serial.println(" ESP32-S3 INVERTER MONITOR");
  Serial.println("============================");

#ifdef TFT_BL
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);
#endif

  Serial.println("Starting TFT_eSPI...");

  tft.init();

  Serial.println("TFT INIT OK");

  tft.setRotation(0);

  // ST7789 color correction
  tft.setSwapBytes(true);

  currentPage = 0;
  drawCurrentPage();
  lastPageChange = millis();

  Serial.println("Page 1 displayed");
}

// =====================================================
// LOOP
// =====================================================
void loop() {

  unsigned long now = millis();

  if (now - lastPageChange >= PAGE_INTERVAL) {
    lastPageChange = now;
    currentPage = (currentPage + 1) % PAGE_COUNT;
    drawCurrentPage();

    Serial.print("Page displayed: ");
    Serial.println(currentPage + 1);
  }

  delay(20);
}
