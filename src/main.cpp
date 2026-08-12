#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

// Backlight
#define TFT_BL 7

TFT_eSPI tft = TFT_eSPI();

void showColor(uint16_t color, const char* name)
{
  tft.fillScreen(color);

  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_WHITE, color);

  tft.drawString(name, 120, 120, 4);

  delay(1000);
}

void setup()
{
  Serial.begin(115200);

  // Backlight
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  // Display
  tft.init();
  tft.setRotation(0);

  // Color test
  showColor(TFT_RED, "RED");
  showColor(TFT_GREEN, "GREEN");
  showColor(TFT_BLUE, "BLUE");

  // Main test screen
  tft.fillScreen(TFT_BLACK);

  tft.setTextDatum(MC_DATUM);

  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString("INVERTER", 120, 55, 4);
  tft.drawString("MONITOR", 120, 85, 4);

  tft.drawFastHLine(30, 110, 180, TFT_DARKGREY);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("ESP32-S3", 120, 135, 2);
  tft.drawString("ST7789 240x240", 120, 158, 2);

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString("DISPLAY TEST", 120, 190, 2);
  tft.drawString("OK", 120, 215, 4);

  Serial.println("ST7789 Display Test Ready");
}

void loop()
{
}
