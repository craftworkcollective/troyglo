// NEO PIXELS
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN 6
#define LED_COUNT 32
#define BRIGHTNESS 50  // Set BRIGHTNESS to about 1/5 (max = 255)
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);

void setup() {
  // Setup neopixels
  strip.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();   // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);
}

void loop() {


  // NEO PIXEL ANIMATION

  animate_gradient_fill(255, 0, 0, 255, 0, 0, 255, 255, 1000);
  animate_gradient_fill(0, 0, 255, 255, 255, 0, 0, 255, 1000);
}


//Linaer interpolate
void animate_gradient_fill(
  uint8_t start_r,
  uint8_t start_g,
  uint8_t start_b,
  uint8_t start_w,
  uint8_t end_r,
  uint8_t end_g,
  uint8_t end_b,
  uint8_t end_w,
  int duration_ms) {
  unsigned long start = millis();
  // start color
  strip.fill(strip.Color(start_r, start_g, start_b, start_w));
  strip.show();

  // start time
  unsigned long delta = millis() - start;

  // animation loop
  while (delta < duration_ms) {
    float pos = (float)delta / (float)duration_ms;
    uint32_t color = color_gradient(start_r, start_g, start_b, start_w, end_r, end_g, end_b, end_w, pos);
    strip.fill(color);
    strip.show();
    delta = millis() - start;
  }

  //end color
  strip.fill(strip.Color(end_r, end_g, end_b, end_w));
  strip.show();
}

uint32_t color_gradient(uint8_t start_r,
                        uint8_t start_g,
                        uint8_t start_b,
                        uint8_t start_w,
                        uint8_t end_r,
                        uint8_t end_g,
                        uint8_t end_b,
                        uint8_t end_w,
                        float pos) {
  uint8_t red = (uint8_t)lerp(pos, 0.0, 1.0, start_r, end_r);
  uint8_t green = (uint8_t)lerp(pos, 0.0, 1.0, start_g, end_g);
  uint8_t blue = (uint8_t)lerp(pos, 0.0, 1.0, start_b, end_b);
  uint8_t white = (uint8_t)lerp(pos, 0.0, 1.0, start_w, end_w);
  return Adafruit_NeoPixel::Color(red, green, blue, white);
}

float lerp(float x, float x0, float x1, float y0, float y1) {
  x = x > x1 ? x1 : x;
  x = x < x0 ? x0 : x;
  return y0 + (y1 - y0) * ((x - x0) / (x1 - x0));
}

// ANIMATIONS
void blueYellow() {
  strip.fill(strip.Color(255, 255, 224, strip.gamma8(255)));
  strip.show();
}

void blueWhite() {
  strip.fill(strip.Color(0, 0, 255, strip.gamma8(255)));
  strip.show();
}

void fullWhite() {
  strip.fill(strip.Color(0, 0, 0, strip.gamma8(255)));
  strip.show();
}

void pulseWhite(uint8_t wait) {
  for (int j = 0; j < 256; j++) {  // Ramp up from 0 to 255
    // Fill entire strip with white at gamma-corrected brightness level 'j':
    strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
    strip.show();
    delay(wait);
  }

  for (int j = 255; j >= 0; j--) {  // Ramp down from 255 to 0
    strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
    strip.show();
    delay(wait);
  }
}
