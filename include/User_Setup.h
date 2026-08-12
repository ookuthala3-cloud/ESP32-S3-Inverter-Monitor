#pragma once

#define ST7789_DRIVER

#define TFT_WIDTH  240
#define TFT_HEIGHT 240

// ESP32-S3 → ST7789
#define TFT_MOSI 11
#define TFT_SCLK 12
#define TFT_CS   10
#define TFT_DC    9
#define TFT_RST   8

#define TFT_RGB_ORDER TFT_BGR

#define TFT_INVERSION_ON

// Fonts
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

#define SMOOTH_FONT

// Start conservatively
#define SPI_FREQUENCY  20000000
