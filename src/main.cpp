#include <Arduino.h>
#include <TFT_eSPI.h>

#define TFT_BL 7

TFT_eSPI tft = TFT_eSPI();

void setup()
{
    Serial.begin(115200);
    delay(1500);

    Serial.println("BOOT OK");

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    Serial.println("BACKLIGHT OK");

    delay(300);

    Serial.println("Starting TFT_eSPI...");

    tft.init();

    Serial.println("TFT_eSPI INIT OK");

    tft.setRotation(0);

    tft.fillScreen(TFT_RED);
    delay(1000);

    tft.fillScreen(TFT_GREEN);
    delay(1000);

    tft.fillScreen(TFT_BLUE);
    delay(1000);

    tft.fillScreen(TFT_BLACK);

    tft.setTextDatum(MC_DATUM);

    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString("INVERTER", 120, 65, 4);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("MONITOR", 120, 100, 4);

    tft.drawFastHLine(30, 125, 180, TFT_DARKGREY);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("TFT_eSPI OK", 120, 165, 4);

    Serial.println("DISPLAY TEST COMPLETE");
}

void loop()
{
    delay(1000);
}
