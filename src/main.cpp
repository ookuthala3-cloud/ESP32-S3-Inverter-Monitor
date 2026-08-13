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
  // Smooth 270-degree arc with the gap centered at the BOTTOM.
  // This avoids TFT_eSPI drawArc angle-orientation differences.
  const float startDeg = 225.0f;   // bottom-left
  const float sweepDeg = 270.0f;   // through left -> top -> right -> bottom-right
  const int thickness = (radius >= 45) ? 9 : 6;
  const uint16_t inactiveColor = COL_BORDER;

  float normalized =
      constrain((value - minValue) / (maxValue - minValue), 0.0f, 1.0f);
  float activeEnd = startDeg + sweepDeg * normalized;

  auto drawSmoothPart = [&](float a0, float a1, uint16_t color) {
    if (a1 <= a0) return;

    // Small angular steps + thick lines = continuous/smooth arc.
    for (float a = a0; a < a1; a += 1.0f) {
      float b = min(a + 1.5f, a1);

      float ar = a * DEG_TO_RAD;
      float br = b * DEG_TO_RAD;

      int x1 = cx + (int)round(cos(ar) * radius);
      int y1 = cy - (int)round(sin(ar) * radius);
      int x2 = cx + (int)round(cos(br) * radius);
      int y2 = cy - (int)round(sin(br) * radius);

      for (int t = 0; t < thickness; t++) {
        float rr = radius - t;
        int tx1 = cx + (int)round(cos(ar) * rr);
        int ty1 = cy - (int)round(sin(ar) * rr);
        int tx2 = cx + (int)round(cos(br) * rr);
        int ty2 = cy - (int)round(sin(br) * rr);
        tft.drawLine(tx1, ty1, tx2, ty2, color);
      }
    }
  };

  // Full inactive arc, then active value over it.
  drawSmoothPart(startDeg, startDeg + sweepDeg, inactiveColor);
  drawSmoothPart(startDeg, activeEnd, gaugeColor);

  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_WHITE, COL_BG);
  tft.setTextSize(1);
  tft.drawString(String(value, decimals), cx, cy - 1, 2);

  tft.setTextColor(COL_DIM, COL_BG);
  tft.drawString(unit, cx, cy + 16, 1);

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
  tft.drawString("BATTERY & DC", 120, 13);
  tft.drawFastHLine(8, 28, 224, COL_BORDER);

  // ---------------------------------------------------
  // Battery gauge: TRUE upper semicircle  ( ∩ )
  // ---------------------------------------------------
  const int cx = 120;
  const int cy = 102;       // baseline/endpoints of the semicircle
  const int radius = 52;
  const int thickness = 9;
  const float startDeg = 180.0f;  // left
  const float sweepDeg = -180.0f; // left -> top -> right
  float normalized = constrain(batteryPct / 100.0f, 0.0f, 1.0f);
  float activeEnd = startDeg + sweepDeg * normalized;

  auto drawSemiPart = [&](float a0, float a1, uint16_t color) {
    float dir = (a1 >= a0) ? 1.0f : -1.0f;
    for (float a = a0; (dir > 0 ? a < a1 : a > a1); a += dir) {
      float b = a + dir * 1.5f;
      if (dir > 0 && b > a1) b = a1;
      if (dir < 0 && b < a1) b = a1;

      float ar = a * DEG_TO_RAD;
      float br = b * DEG_TO_RAD;

      for (int t = 0; t < thickness; t++) {
        float rr = radius - t;
        int x1 = cx + (int)round(cos(ar) * rr);
        int y1 = cy + (int)round(sin(ar) * rr);
        int x2 = cx + (int)round(cos(br) * rr);
        int y2 = cy + (int)round(sin(br) * rr);
        tft.drawLine(x1, y1, x2, y2, color);
      }
    }
  };

  // Grey full upper arc, green active part.
  drawSemiPart(startDeg, 0.0f, COL_BORDER);
  drawSemiPart(startDeg, activeEnd, COL_GREEN);

  // Center text lives INSIDE the semicircle.
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(COL_TEXT, COL_BG);
  tft.setTextFont(4);
  tft.drawString(String(batteryPct), 111, 77);

  tft.setTextFont(2);
  tft.drawString("%", 148, 79);
  tft.drawString(String(batteryVolt, 1) + " V", 120, 98);

  // ---------------------------------------------------
  // MIN / MAX / AVG cards
  // ---------------------------------------------------
  const int cardY = 112;
  const int cardW = 70;
  const int cardH = 36;
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
                   cardX[i] + cardW / 2, cardY + 25);
  }

  // ---------------------------------------------------
  // Discharge current
  // ---------------------------------------------------
  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(1);
  tft.setTextColor(COL_DIM, COL_BG);
  tft.drawString("DISCHARGE CURRENT", 10, 155);

  tft.setTextDatum(TR_DATUM);
  tft.setTextColor(COL_TEXT, COL_BG);
  tft.drawString(String(batteryCurrent, 1) + " A", 230, 155);

  const int barX = 10;
  const int barY = 169;
  const int segW = 18;
  const int segH = 8;
  const int gap = 4;
  const int barSegments = 10;

  int active = constrain(
      (int)round((batteryCurrent / 10.0f) * barSegments),
      0, barSegments);

  for (int i = 0; i < barSegments; i++) {
    uint16_t c = (i < active) ? COL_GREEN : COL_BORDER;
    tft.fillRoundRect(barX + i * (segW + gap), barY,
                      segW, segH, 2, c);
  }

  // ---------------------------------------------------
  // Footer safely inside 240x240
  // ---------------------------------------------------
  tft.drawFastHLine(8, 188, 224, COL_BORDER);
  drawBattery(12, 199, batteryPct);

  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(1);
  tft.setTextColor(COL_DIM, COL_BG);
  tft.drawString("STATUS", 68, 205);

  tft.setTextColor(COL_GREEN, COL_BG);
  tft.drawString("NORMAL", 108, 205);

  tft.fillCircle(218, 209, 7, COL_GREEN);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_BLACK, COL_GREEN);
  tft.drawString("OK", 218, 209, 1);

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
