#include <math.h>
#include <Adafruit_NeoPixel.h>
#include <stdlib.h>
#include "ws2812_program.h"
#include "utils.h"
#include "simplex_noise.h"

void SpectralProgram::iterate(Adafruit_NeoPixel &strip, float time) {
  strip.fill(strip.ColorHSV(uint16_t(fmod(time * this->speed, 1.0) * 65536), 255, 255));
}

void RainbowWaveProgram::iterate(Adafruit_NeoPixel &strip, float time) {
  for (int x = 0; x < strip.numPixels(); x++) {
    float val = this->n_waves * (fmod(time * this->speed, 1.0) + x / float(strip.numPixels()));
    strip.setPixelColor(strip.numPixels() - x - 1, strip.ColorHSV(uint16_t(val * 65536), 255, 255));
  }
}

RandomStarsProgram::RandomStarsProgram(float speed, uint16_t hue, uint8_t sat, int led_count) :
  WS2812Program(speed), hue(hue), sat(sat), led_count(led_count) {
  for(int i = 0; i < led_count; i++) {
    this->strip_states.push_back(0);
    this->strip_intensities.push_back(0);
  }
}

void RandomStarsProgram::iterate(Adafruit_NeoPixel &strip, float time) {
  for (int i = 0; i < this->generation_attemps_per_frame; i++) {  // Creating new stars
    if ((rand() % 100) < this->new_star_probability) {
      uint8_t index = rand() % this->led_count;
      if (this->strip_states[index] == 0) {this->strip_states[index] = 1;}
    }
  }

  for (int x = 0; x < this->led_count; x++) {
    if (this->strip_states[x] == 1) { // States when the intensity should increase
      if (this->strip_intensities[x] >= 255)  // if it reached maximum intensity
        this->strip_states[x] = 2;  // It enters the decreasing state
      else {
        this->strip_intensities[x] += 1 * this->speed;
        this->strip_intensities[x] = min(255, this->strip_intensities[x]);
      }

    } else if (this->strip_states[x] == 2) { // States when the intensity should decrease
      if (this->strip_intensities[x] <= 0)  // if it reached minimum intensity
        this->strip_states[x] = 0;  // It enters the inactive state
      else {
        this->strip_intensities[x] -= 1 * this->speed;
        this->strip_intensities[x] = max(0, this->strip_intensities[x]);
      }
    }
  }

  for (int x = 0; x < strip.numPixels(); x++) {
    strip.setPixelColor(x, strip.ColorHSV(this->hue, this->sat, (uint8_t)this->strip_intensities[x])); 
  }
}


void DiffuseSparklingProgram::iterate(Adafruit_NeoPixel &strip, float time) {
  float intensity;
  for (int x = 0; x < strip.numPixels(); x++) {
    intensity = 0.5 * (1 + SimplexNoise::noise(x/this->scale, time * this->speed));
    intensity = max(intensity * intensity, 0.025f);
    strip.setPixelColor(x, strip.ColorHSV(this->hue, this->sat, (uint8_t)(intensity * 255)));
  } 
}

void ChristmasTreeProgram::iterate(Adafruit_NeoPixel &strip, float time) {
  float bg_intensity;  // Blue and Green intensities
  float ry_intensity;  // Red and Yellow intensities
  float _t = fmod(time, 244);

  if (_t < 90) {
    bg_intensity = 0.5f * (1 + sin(2 * time * PI / 15 - PI/2)) * squareWave(2 * time * PI / 15, 4*PI);
    ry_intensity = 0.5f * (1 + sin(2 * time * PI / 15 - PI/2)) * (1 - squareWave(2 * time * PI / 15, 4*PI));
  }
  else if (_t < 120) {
    bg_intensity = squareWave(time, 4);
    ry_intensity = squareWave(time + 2, 4);
  }
  else if (_t < 140) {
    bg_intensity = squareWave(time, 2);
    ry_intensity = squareWave(time + 1, 2);
  }
  else if (_t < 150) {
    bg_intensity = squareWave(time, 1);
    ry_intensity = squareWave(time + 0.5f, 1);
  }
  else if (_t < 155) {
    bg_intensity = squareWave(time, 0.5f);
    ry_intensity = squareWave(time + 0.25f, 0.5f);
  }
  else if (_t < 156) {
    bg_intensity = squareWave(time, 0.25);
    ry_intensity = 0;
  }
  else if (_t < 157) {
    bg_intensity = 0;
    ry_intensity = squareWave(time, 0.25);
  }
  else if (_t < 162) {
    bg_intensity = squareWave(time, 0.5f);
    ry_intensity = squareWave(time + 0.25f, 0.5f);
  }
  else if (_t < 163) {
    bg_intensity = squareWave(time, 0.25);
    ry_intensity = 0;
  }
  else if (_t < 164) {
    bg_intensity = 0;
    ry_intensity = squareWave(time, 0.25);
  }
  else if (_t < 194) {
    bg_intensity = 0.5f * (1 + sin(2 * time * PI / 15 - PI/2));
    ry_intensity = 0.5f * (1 + sin(2 * time * PI / 15 - PI/2));
  }
  else if (_t <= 244) {
    bg_intensity = 1;
    ry_intensity = 1;
  }

  for (int x = 0; x < strip.numPixels(); x+=4) strip.setPixelColor(x, strip.ColorHSV(this->BLUE_HUE, 255, (uint8_t)(255 * bg_intensity)));
  for (int x = 1; x < strip.numPixels(); x+=4) strip.setPixelColor(x, strip.ColorHSV(this->YELLOW_HUE, 255, (uint8_t)(255 * ry_intensity)));
  for (int x = 2; x < strip.numPixels(); x+=4) strip.setPixelColor(x, strip.ColorHSV(this->GREEN_HUE, 255, (uint8_t)(255 * bg_intensity)));
  for (int x = 3; x < strip.numPixels(); x+=4) strip.setPixelColor(x, strip.ColorHSV(this->RED_HUE, 255, (uint8_t)(255 * ry_intensity)));
}

void BiColorPerlinProgram::iterate(Adafruit_NeoPixel &strip, float time) {
  float intensity;
  for (int x = 0; x < strip.numPixels(); x++) {
    intensity = SimplexNoise::noise(x/this->scale, time * this->speed);  // Is between -1 and 1
    if (intensity > 0) {
      //intensity = max(intensity * intensity, 0.025f);
      strip.setPixelColor(x, strip.ColorHSV(this->hue_1, this->sat_1, (uint8_t)(intensity * 255)));
    }
    else {
      //intensity = max(intensity * intensity, 0.025f);
      strip.setPixelColor(x, strip.ColorHSV(this->hue_2, this->sat_2, (uint8_t)(-intensity * 255)));
    }
  } 
}

void TriColorPerlinProgram::iterate(Adafruit_NeoPixel &strip, float time) {
  float value;
  uint32_t color;
  for (int x = 0; x < strip.numPixels(); x++) {
    value = SimplexNoise::noise(x/this->scale, time * this->speed);  // Is between -1 and 1
    if (value > 0) {
      color = interpolateColors888(
        strip.ColorHSV(this->hue_2, this->sat_2, 255),
        strip.ColorHSV(this->hue_1, this->sat_1, 255),
        min(1.5f * value, 1)
      );
    }
    else {
      color = interpolateColors888(
        strip.ColorHSV(this->hue_2, this->sat_2, 255),
        strip.ColorHSV(this->hue_3, this->sat_3, 255),
        min(-1.5f * value, 1)
      );
    }
    strip.setPixelColor(x, color);
  } 
}