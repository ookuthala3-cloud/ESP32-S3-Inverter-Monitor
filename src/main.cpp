#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

#define TFT_BL 4

static const uint16_t BG     = TFT_BLACK;
static const uint16_t ACTIVE = TFT_GREEN;
static const uint16_t TRACK  = 0x7BEF;   // grey

// Draw a guaranteed upper semicircle: left -> top -> right.
// We calculate every pixel/line ourselves instead of depending on drawArc angle conventions.
void drawUpperGauge(int cx, int cy, int radius, int thickness, float percent) {
  percent = constrain(percent, 0.0f, 100.0f);

  auto drawPart = [&](float startDeg, float endDeg, uint16_t color) {
    if (endDeg < startDeg) return;

    for (float deg = startDeg; deg <= endDeg; deg += 0.75f) {
      float rad = deg * DEG_TO_RAD;

      // Mathematical coordinates converted to LCD coordinates.
      // 180° = left, 90° = top, 0° = right.
      for (int t = 0; t < thickness; t++) {
        float r = radius - t;
        int x = cx + (int)roundf(cosf(rad) * r);
        int y = cy - (int)roundf(sinf(rad) * r);
        tft.drawPixel(x, y, color);
      }
    }
  };

  // Entire upper semicircle background.
  drawPart(0.0f, 180.0f, TRACK);

  // Value should grow LEFT -> TOP -> RIGHT.
  // For 82%, endpoint moves from 180° down toward 0°.
  float endAngle = 180.0f - (percent / 100.0f) * 180.0f;

  // drawPart works increasing, so active range is endpoint -> 180.
  drawPart(endAngle, 180.0f, ACTIVE);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  Serial.println();
  Serial.println("===== UPPER GAUGE GEOMETRY TEST =====");

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(BG);

  // Header/reference baseline
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_GREEN, BG);
  tft.drawString("GAUGE TEST", 120, 18, 2);
  tft.drawFastHLine(20, 34, 200, TFT_DARKGREY);

  // Gauge endpoints are at y=145.
  // Top of gauge = 145 - 70 = 75.
  drawUpperGauge(120, 145, 70, 10, 82.0f);

  // Center labels
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_WHITE, BG);
  tft.drawString("82%", 120, 116, 4);
  tft.drawString("12.6 V", 120, 142, 2);

  // Endpoint markers help verify geometry.
  tft.fillCircle(50, 145, 2, TFT_RED);
  tft.fillCircle(190, 145, 2, TFT_RED);

  tft.setTextColor(TFT_LIGHTGREY, BG);
  tft.drawString("Expected shape: upper arch", 120, 190, 2);
  tft.drawString("gap/open area below", 120, 210, 2);

  Serial.println("Gauge test drawn.");
}

void loop() {
}
