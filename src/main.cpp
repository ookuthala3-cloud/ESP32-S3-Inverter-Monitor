#include <Arduino.h>
#include <TFT_eSPI.h>

#define BACKLIGHT_PIN 7

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("BOOT OK");

  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, HIGH);

  Serial.println("BL OK");

  delay(500);

  Serial.println("Starting TFT...");
  tft.init();
  Serial.println("TFT INIT OK");

  tft.setRotation(0);

  Serial.println("RED");
  tft.fillScreen(TFT_RED);
  delay(2000);

  Serial.println("GREEN");
  tft.fillScreen(TFT_GREEN);
  delay(2000);

  Serial.println("BLUE");
  tft.fillScreen(TFT_BLUE);
  delay(2000);

  Serial.println("BLACK");
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("ESP32-S3", 120, 95, 4);

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString("DISPLAY OK", 120, 135, 4);

  Serial.println("TEST COMPLETE");
}

void loop() {
  delay(1000);
}
