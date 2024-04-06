#ifndef WS2812_PROGRAM_H
#define WS2812_PROGRAM_H
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <vector>
#include <tuple>
#include <math.h>
#include "utils.h"
#include "simplex_noise.h"


class WS2812Program {
  public:
    float speed;
    WS2812Program(float speed){this->speed = speed;};
    virtual void iterate(Adafruit_NeoPixel &strip, float time);
};


class StaticProgram: public WS2812Program {
  private:
    uint32_t color;
  public:
    StaticProgram(uint32_t color) : WS2812Program(0), color(color) {};
    void iterate(Adafruit_NeoPixel &strip, float time) {strip.fill(this->color);}
};


class SpectralProgram: public WS2812Program {
  public:
    SpectralProgram(float speed) : WS2812Program(speed) {};
    void iterate(Adafruit_NeoPixel &strip, float time);
};

class RainbowWaveProgram: public WS2812Program {
  public:
    int n_waves = 1;

    RainbowWaveProgram(float speed, int n_waves) : WS2812Program(speed), n_waves(n_waves) {};
    void iterate(Adafruit_NeoPixel &strip, float time);
};

class RandomStarsProgram: public WS2812Program {
  private:
    uint16_t hue;
    uint8_t sat;
    uint8_t led_count;
    std::vector<uint8_t> strip_states;
    std::vector<float> strip_intensities;
    uint8_t generation_attemps_per_frame = 3;
    uint8_t new_star_probability = 5;  //Percentage
  public:
    RandomStarsProgram(float speed, uint16_t hue, uint8_t sat, int led_count);
    void iterate(Adafruit_NeoPixel &strip, float time);
};


class DiffuseSparklingProgram: public WS2812Program {
  private:
    float scale;
    uint16_t hue;
    uint8_t sat;
  public:
    DiffuseSparklingProgram(float speed, float scale, uint16_t hue, uint8_t sat) :
      WS2812Program(speed), scale(scale), hue(hue), sat(sat) {};
    void iterate(Adafruit_NeoPixel &strip, float time);
};


class ChristmasTreeProgram: public WS2812Program {
  private:
    const uint16_t BLUE_HUE = 43690;
    const uint16_t GREEN_HUE = 0;
    const uint16_t RED_HUE = 21845;
    const uint16_t YELLOW_HUE = 19500;
  public:
    ChristmasTreeProgram(float speed) : WS2812Program(speed) {};
    void iterate(Adafruit_NeoPixel &strip, float time);
};


class BiColorPerlinProgram: public WS2812Program {
  private:
    float scale;
    uint16_t hue_1;
    uint16_t hue_2;
    uint8_t sat_1;
    uint8_t sat_2;
  public:
    BiColorPerlinProgram(float speed, float scale, uint16_t hue_1, uint16_t hue_2, uint8_t sat_1, uint8_t sat_2) :
      WS2812Program(speed), scale(scale), hue_1(hue_1), hue_2(hue_2), sat_1(sat_1), sat_2(sat_2) {};
    void iterate(Adafruit_NeoPixel &strip, float time);
};

class TriColorPerlinProgram: public WS2812Program {
  private:
    float scale;
    uint16_t hue_1;
    uint16_t hue_2;
    uint16_t hue_3;
    uint8_t sat_1;
    uint8_t sat_2;
    uint8_t sat_3;
  public:
    TriColorPerlinProgram(float speed, float scale, uint16_t hue_1, uint16_t hue_2, uint16_t hue_3, uint8_t sat_1, uint8_t sat_2, uint8_t sat_3) :
      WS2812Program(speed), scale(scale), hue_1(hue_1), hue_2(hue_2), hue_3(hue_3), sat_1(sat_1), sat_2(sat_2), sat_3(sat_3) {};
    void iterate(Adafruit_NeoPixel &strip, float time);
};

class RainbowTwinkleProgram: public WS2812Program {
  private:
    uint8_t generation_attemps_per_frame = 3;
    uint8_t new_star_probability = 20;  //Percentage
  public:
    RainbowTwinkleProgram(uint8_t generation_attemps_per_frame, uint8_t new_star_probability) :
      WS2812Program(0), generation_attemps_per_frame(generation_attemps_per_frame), new_star_probability(new_star_probability) {};
    void iterate(Adafruit_NeoPixel &strip, float time);
};

class CometsProgram: public WS2812Program { // Inspired by https://github.com/davepl/DavesGarageLEDSeries/blob/master/LED%20Episode%2006/src/comet.h
  private:
    const uint8_t comet_size[3] = {5, 4, 3};
    int direction[3] = {1, 1, 1};
    float position[3] = {0, 20, 40};
    float speed_multiplier[3] = {1.0f, 1.1f, 1.2f};
    uint8_t n_comets;
    const float fade_factor = 0.75f;
  public:
    CometsProgram(float speed, uint8_t n_comets) : WS2812Program(speed), n_comets(n_comets) {};
    void iterate(Adafruit_NeoPixel &strip, float time);
};
#endif
