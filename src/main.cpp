#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup()
{
    Serial.begin(115200);
    delay(1500);

    Serial.println();
    Serial.println("===== TFT_eSPI SPI TEST =====");
    Serial.println("BOOT OK");

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);

    Serial.println("BACKLIGHT OK");

    // Explicitly initialise Arduino SPI first
    Serial.println("Starting SPI...");

    SPI.begin(
        TFT_SCLK,   // GPIO 5
        TFT_MISO,   // GPIO 41
        TFT_MOSI,   // GPIO 6
        TFT_CS      // GPIO 16
    );

    Serial.println("SPI BEGIN OK");

    delay(500);

    Serial.println("Starting TFT_eSPI...");

    tft.init();

    Serial.println("TFT_eSPI INIT OK");

    tft.setRotation(1);

    Serial.println("RED");
    tft.fillScreen(TFT_RED);
    delay(1500);

    Serial.println("GREEN");
    tft.fillScreen(TFT_GREEN);
    delay(1500);

    Serial.println("BLUE");
    tft.fillScreen(TFT_BLUE);
    delay(1500);

    tft.fillScreen(TFT_BLACK);

    tft.setTextDatum(MC_DATUM);

    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString("INVERTER", 120, 65, 4);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("MONITOR", 120, 100, 4);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("TFT_eSPI OK", 120, 160, 4);

    Serial.println("DISPLAY TEST COMPLETE");
}

void loop()
{
    delay(1000);
}
