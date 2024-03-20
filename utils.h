#ifndef UTILS_H
#define UTILS_H
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

uint16_t ColorHSV(uint16_t hue, uint8_t sat, uint8_t val);
float stripCurrentDraw(Adafruit_NeoPixel &strip, float current_per_channel);
float stripPowerDraw(Adafruit_NeoPixel &strip, float current_per_channel, float voltage);
void stripApplyBrightness(Adafruit_NeoPixel &strip, float brightness);
uint32_t color565To888(uint16_t color565);
uint16_t color888To565(uint32_t color888);
uint16_t interpolateColors565(uint16_t col1, uint16_t col2, float frac); // Interpolates between 2 16 bits colors
uint32_t interpolateColors888(uint32_t col_1, uint32_t col_2, float frac); // Interpolates between 2 32 bits colors
uint32_t fadeColor(uint32_t color888, float factor);
uint8_t squareWave(float time, float period);

#endif