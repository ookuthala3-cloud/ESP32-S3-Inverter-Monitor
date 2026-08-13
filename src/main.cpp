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

float inverterTemp  = 38.0;

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
void drawCenteredText(
  const String &text,
  int x,
  int y,
  int width,
  uint16_t color,
  uint8_t size = 1
) {
  tft.setTextColor(color, COL_BG);
  tft.setTextSize(size);

  int16_t x1, y1;
  uint16_t w, h;

  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  tft.setCursor(x + (width - w) / 2, y);
  tft.print(text);
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
  uint8_t decimals
) {
  const int startAngle = 135;
  const int endAngle   = 405;

  // background arc
  for (int a = startAngle; a <= endAngle; a += 5) {

    float rad = a * DEG_TO_RAD;

    int x1 = cx + cos(rad) * (radius - 4);
    int y1 = cy + sin(rad) * (radius - 4);

    int x2 = cx + cos(rad) * radius;
    int y2 = cy + sin(rad) * radius;

    tft.drawLine(x1, y1, x2, y2, 0x4208);
  }

  float normalized =
      constrain((value - minValue) / (maxValue - minValue), 0.0, 1.0);

  int valueAngle =
      startAngle + normalized * (endAngle - startAngle);

  // active arc
  for (int a = startAngle; a <= valueAngle; a += 4) {

    float rad = a * DEG_TO_RAD;

    int x1 = cx + cos(rad) * (radius - 5);
    int y1 = cy + sin(rad) * (radius - 5);

    int x2 = cx + cos(rad) * radius;
    int y2 = cy + sin(rad) * radius;

    tft.drawLine(x1, y1, x2, y2, gaugeColor);
  }

  // value
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_WHITE, COL_BG);
  tft.setTextSize(1);

  String valueText = String(value, decimals);

  tft.drawString(valueText, cx, cy - 1, 2);

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

  drawMainPage();

  Serial.println("Page 1 displayed");
}

// =====================================================
// LOOP
// =====================================================
void loop() {

  // UI only test for now.
  // Sensors + page rotation will be added next.

  delay(100);
}
