#include <math.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <stdlib.h>
#include "utils.h"
#include "ws2812_program.h"
#include "RotaryEncoder.h"
#include "ezButton.h"
#include "config_save.h"

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif


#define NUMBER_OF_PROGRAMS 10
#define FRAMERATE 31  // Frames per second
#define VOLTAGE 5  // Volts
#define CURRENT_DRAW_PER_CHANNEL 0.020  // Amperes
#define MAX_CURRENT_DRAW 2.0 // Amperes
#define ROT_ENC_BUTTON_PIN 16 
#define ROTARY_ENC_DT_PIN 17
#define ROT_ENC_CLK_PIN 18

#define LED_PIN 19

#define LED_COUNT 50

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
RotaryEncoder rotary_encoder(ROT_ENC_CLK_PIN, ROTARY_ENC_DT_PIN, RotaryEncoder::LatchMode::TWO03);
ezButton button(ROT_ENC_BUTTON_PIN);


const float timestep = 1.0 / FRAMERATE;
float brightness = 0.1f;
unsigned long t0;
unsigned long t1;
unsigned long t2;  // Time we will dedicate to continuously refresh the rotary encoder so that we get a smooth use of the encoder.
double t = 0.0f;
double time_of_last_encoder_use = 0.0f;
float current_draw;
RotaryEncoder::Direction rotary_encoder_direction;
int selected_program = 0;
bool is_selecting_program = false;  // If true : selects the program. If false. Selects the brightness
bool button_has_been_released = true;


WS2812Program *programs[NUMBER_OF_PROGRAMS];
SpectralProgram spectral_prog = SpectralProgram(0.025);
RainbowWaveProgram rainbow_wave_prog = RainbowWaveProgram(0.2, 1);
StaticProgram yellow_static_prog = StaticProgram(strip.ColorHSV(19500, 251, 255));
DiffuseSparklingProgram gold_diffuse_sparkling_prog = DiffuseSparklingProgram(0.5f, 5.0f, 19500, 251);
DiffuseSparklingProgram silver_diffuse_sparkling_prog = DiffuseSparklingProgram(0.5f, 5.0f, 65535, 0);
ChristmasTreeProgram christmas_tree_prog = ChristmasTreeProgram(1.0f);
BiColorPerlinProgram bi_color_perlin_prog = BiColorPerlinProgram(0.25f, 10.0f, 21845, 0, 255, 255);
TriColorPerlinProgram tri_color_perlin_prog = TriColorPerlinProgram(0.25f, 20.0f, 21845, 65535, 0, 255, 0, 255);
RainbowTwinkleProgram rainbow_twinkle_prog = RainbowTwinkleProgram(3, 50);
CometsProgram comets_prog = CometsProgram(0.01f, 3);

// setup() function -- runs once at startup --------------------------------
void setup() {
  randomSeed(micros() + analogRead(LED_PIN));
  t += random(3600);

  programs[0] = &rainbow_wave_prog;
  programs[1] = &spectral_prog;
  programs[2] = &yellow_static_prog;
  programs[3] = &gold_diffuse_sparkling_prog;
  programs[4] = &silver_diffuse_sparkling_prog;
  programs[5] = &christmas_tree_prog;
  programs[6] = &bi_color_perlin_prog;
  programs[7] = &tri_color_perlin_prog;
  programs[8] = &rainbow_twinkle_prog;
  programs[9] = &comets_prog;

  AppConfig* config = loadConfig();
  brightness = config->brightness;
  selected_program = config->selected_program;
  brightness = max(0.1f, min(1, brightness));
  selected_program = max(0, min(NUMBER_OF_PROGRAMS - 1, selected_program));

  strip.begin();
  strip.setBrightness(255);

  // Colorwipe animation to act as boot up animation
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0, 140, 255); //  GRB
    strip.show();
    strip.setPixelColor(i, 0);
    delay(25);
  }
  delay(500);
  strip.fill(0);
}


void loop() {
  t0 = millis();

  programs[selected_program]->iterate(strip, t);
  stripApplyBrightness(strip, brightness);
  current_draw = stripCurrentDraw(strip, CURRENT_DRAW_PER_CHANNEL);
  if (current_draw > MAX_CURRENT_DRAW) {
    brightness = brightness * (MAX_CURRENT_DRAW / current_draw) - 0.01;
    stripApplyBrightness(strip, brightness);
  }
  strip.show();

  t1 = millis();
  t2 = millis();
  while (timestep * 1000 - (t2 - t0) > 0){
    button.loop();
    if (button.isPressed() && button_has_been_released && (t - time_of_last_encoder_use) > 0.30f) {
      button_has_been_released = false;
      is_selecting_program = !is_selecting_program;
      time_of_last_encoder_use = t;
      strip.setPixelColor(0, 0, 140, 255);
      strip.setPixelColor(1, 0, 140, 255);
      strip.setPixelColor(2, 0, 140, 255);
      strip.show();
    }
    if (!button.isPressed()) {
      button_has_been_released = true;
    }

    rotary_encoder.tick();
    rotary_encoder_direction = rotary_encoder.getDirection();
    if (
      rotary_encoder_direction != RotaryEncoder::Direction::NOROTATION
      && (t - time_of_last_encoder_use) > 0.05f
      ) {
      if (is_selecting_program) {
        strip.fill(0);
        switch (rotary_encoder_direction) {
          case RotaryEncoder::Direction::CLOCKWISE:
            selected_program -= 1;
            break;
          case RotaryEncoder::Direction::COUNTERCLOCKWISE:
            selected_program += 1;
            break;
        }
        selected_program = (selected_program + NUMBER_OF_PROGRAMS) % NUMBER_OF_PROGRAMS;
      }
      else {
        switch (rotary_encoder_direction) {
          case RotaryEncoder::Direction::CLOCKWISE:
            brightness /= 1.3;
            break;
          case RotaryEncoder::Direction::COUNTERCLOCKWISE:
            brightness *= 1.3;
            break;
        }
        brightness = max(0.01, min(1.0, brightness));
      }
      AppConfig config;
      config.selected_program = selected_program;
      config.brightness = brightness;
      saveConfig(config);
      time_of_last_encoder_use = t;
    }

  t2 = millis();
  }

  
  //delay(timestep * 1000 - (t1 - t0));

  Serial.print(stripCurrentDraw(strip, CURRENT_DRAW_PER_CHANNEL)); Serial.print(" A | ");
  Serial.print(stripPowerDraw(strip, CURRENT_DRAW_PER_CHANNEL, VOLTAGE)); Serial.print(" W | ");
  char c[2];
  sprintf(c, "%2d", t1 - t0);
  Serial.print("Time spent in cycle : "); Serial.print(c); Serial.print("ms | ");
  Serial.print(1000.0 / (millis() - t0)); Serial.print(" fps | ");
  Serial.print("Brightness : "); Serial.print(brightness); Serial.print(" | ");
  Serial.print("Program n° : "); Serial.print(selected_program); Serial.print(" | ");
  if (is_selecting_program)
    Serial.print("Mode : program selection | ");
  else
    Serial.print("Mode : brightness selection | ");
  Serial.println("");

  t += timestep;
}
