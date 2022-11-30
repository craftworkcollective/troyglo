//HALL EFFECT SENSOR
#include <util/atomic.h>
volatile int revolutions;
int revValue;
float rpm;
int rmpMap;
unsigned long threshold;
unsigned long timerStart;

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
  // Setup hall effect
  Serial.begin(9600);
  attachInterrupt(0, magnet_detect, RISING);  //Initialize the intterrupt pin (Arduino digital pin 2)
  revolutions = 0;
  rpm = 0;
  rmpMap = 0;
  timerStart = 0;
  threshold = 1000;  // 1 second

  // Setup neopixels
  strip.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();   // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);
}

uint8_t start_r = 255;
uint8_t start_b = 0;
uint8_t end_r = 255;
uint8_t end_b = 0;

void loop() {

  // HALL EFFECT DATA
  updateHallEffect();

  // MAP RMP
  rmpMap = map(rpm, 0, 420, 0, 255);
  rmpMap = constrain(rmpMap, 0, 255);
  //Serial.print("rmpMap: ");
  //Serial.println(rmpMap);

  // NEO PIXEL ANIMATION
  end_r = 255 - rmpMap; 
  end_b = rmpMap;
  animate_gradient_fill(start_r, 0, start_b, 255, end_r, 0, end_b, 255, 1000);
  start_r = end_r; 
  start_b = end_b; 
}

// HALL EFFECT
void updateHallEffect() {

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    revValue = revolutions;
  }

  unsigned long difference = millis() - timerStart;

  if (difference >= threshold) {
    Serial.println("revValue: " + (String)revValue);

    // 1 rev / 1 ms X 1000 ms / 1 s X 60 s / 1 min
    rpm = 1000.0 * 60.0 / (float)threshold * (float)revValue;
    timerStart = millis();
    revolutions = 0;
    Serial.println("rpm: " + (String)rpm);

  }
}

void magnet_detect()  //This function is called whenever a magnet/interrupt is detected by the arduino
{
  revolutions++;
  //Serial.println(String(revolutions) + " detect");
}

// ANIMATIONS//Linaer interpolate
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