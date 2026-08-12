#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// ===== ST7789 Pins =====
#define TFT_MOSI  11
#define TFT_SCLK  12
#define TFT_CS    10
#define TFT_DC     9
#define TFT_RST    8
#define TFT_BL     7

SPIClass displaySPI(FSPI);

Adafruit_ST7789 tft =
  Adafruit_ST7789(&displaySPI, TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);
  delay(1500);

  Serial.println();
  Serial.println("=== INVERTER MONITOR DISPLAY TEST ===");
  Serial.println("BOOT OK");

  // Backlight
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  Serial.println("BACKLIGHT OK");

  // Explicit SPI pins
  displaySPI.begin(
    TFT_SCLK,   // SCK
    -1,         // MISO - not used
    TFT_MOSI,   // MOSI
    TFT_CS
  );

  Serial.println("SPI BEGIN OK");

  delay(300);

  // 240 x 240 ST7789
  tft.init(240, 240);

  Serial.println("TFT INIT OK");

  tft.setRotation(0);

  // RED
  Serial.println("TEST RED");
  tft.fillScreen(ST77XX_RED);
  delay(1500);

  // GREEN
  Serial.println("TEST GREEN");
  tft.fillScreen(ST77XX_GREEN);
  delay(1500);

  // BLUE
  Serial.println("TEST BLUE");
  tft.fillScreen(ST77XX_BLUE);
  delay(1500);

  // Final screen
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextWrap(false);

  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(3);
  tft.setCursor(38, 45);
  tft.println("INVERTER");

  tft.setCursor(55, 75);
  tft.println("MONITOR");

  tft.drawLine(
    20, 115,
    220, 115,
    ST77XX_WHITE
  );

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);

  tft.setCursor(65, 135);
  tft.println("ESP32-S3");

  tft.setCursor(38, 160);
  tft.println("ST7789 240x240");

  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);

  tft.setCursor(48, 195);
  tft.println("DISPLAY OK");

  Serial.println("DISPLAY TEST COMPLETE");
}

void loop() {
  delay(1000);
}
