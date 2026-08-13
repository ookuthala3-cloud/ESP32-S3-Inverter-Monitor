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
  const float startAngle = 135.0;
  const float endAngle   = 405.0;

  // Fewer, clearer segments for the 240x240 LCD.
  const int SEGMENTS = 18;
  const float stepAngle = (endAngle - startAngle) / (SEGMENTS - 1);

  float normalized =
      constrain((value - minValue) / (maxValue - minValue), 0.0, 1.0);

  int activeSegments =
      (int)round(normalized * (SEGMENTS - 1));

  for (int i = 0; i < SEGMENTS; i++) {
    float angle = startAngle + (i * stepAngle);
    float rad = angle * DEG_TO_RAD;

    // Slightly longer segment marks, with visible gaps.
    int x1 = cx + cos(rad) * (radius - 7);
    int y1 = cy + sin(rad) * (radius - 7);

    int x2 = cx + cos(rad) * radius;
    int y2 = cy + sin(rad) * radius;

    uint16_t color = (i <= activeSegments) ? gaugeColor : 0x4208;

    // 2 px-ish thickness makes each separated mark easier to see.
    tft.drawLine(x1, y1, x2, y2, color);

    float rad2 = (angle + 1.8) * DEG_TO_RAD;
    int xx1 = cx + cos(rad2) * (radius - 7);
    int yy1 = cy + sin(rad2) * (radius - 7);
    int xx2 = cx + cos(rad2) * radius;
    int yy2 = cy + sin(rad2) * radius;
    tft.drawLine(xx1, yy1, xx2, yy2, color);
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

  // Large segmented battery gauge
  drawGauge(
    120, 84,
    58,
    batteryPct,
    0,
    100,
    COL_GREEN,
    "%",
    0
  );

  // Battery voltage below the gauge
  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(COL_TEXT, COL_BG);
  tft.drawString(String(batteryVolt, 1) + " V", 120, 119);

  // MIN / MAX / AVG cards
  const int cardY = 137;
  const int cardW = 70;
  const int cardH = 42;
  const int cardX[3] = {8, 85, 162};
  const char* labels[3] = {"MIN", "MAX", "AVG"};
  float vals[3] = {batteryMinV, batteryMaxV, batteryAvgV};

  for (int i = 0; i < 3; i++) {
    tft.drawRoundRect(cardX[i], cardY, cardW, cardH, 4, COL_BORDER);
    tft.setTextDatum(MC_DATUM);
    tft.setTextFont(1);
    tft.setTextColor(COL_DIM, COL_BG);
    tft.drawString(labels[i], cardX[i] + cardW / 2, cardY + 10);

    tft.setTextFont(2);
    tft.setTextColor(COL_TEXT, COL_BG);
    tft.drawString(String(vals[i], 1) + "V",
                   cardX[i] + cardW / 2, cardY + 28);
  }

  // Current label
  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(1);
  tft.setTextColor(COL_DIM, COL_BG);
  tft.drawString("DISCHARGE CURRENT", 10, 188);

  tft.setTextDatum(TR_DATUM);
  tft.setTextColor(COL_TEXT, COL_BG);
  tft.drawString(String(batteryCurrent, 1) + " A", 230, 188);

  // Segmented current bar
  const int barX = 10;
  const int barY = 203;
  const int segW = 19;
  const int segH = 9;
  const int gap = 3;
  const int barSegments = 10;
  int active = constrain((int)round((batteryCurrent / 10.0) * barSegments),
                         0, barSegments);

  for (int i = 0; i < barSegments; i++) {
    uint16_t c = (i < active) ? COL_GREEN : COL_BORDER;
    tft.fillRoundRect(barX + i * (segW + gap), barY,
                      segW, segH, 2, c);
  }

  // Status footer
  tft.drawFastHLine(8, 219, 224, COL_BORDER);
  drawBattery(12, 224, batteryPct);

  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(1);
  tft.setTextColor(COL_DIM, COL_BG);
  tft.drawString("STATUS", 68, 226);

  tft.setTextColor(COL_GREEN, COL_BG);
  tft.drawString("NORMAL", 108, 226);

  tft.fillCircle(218, 231, 7, COL_GREEN);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_BLACK, COL_GREEN);
  tft.drawString("OK", 218, 231, 1);

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
