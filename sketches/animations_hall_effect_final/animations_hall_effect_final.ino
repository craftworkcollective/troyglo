//HALL EFFECT SENSOR
#include <util/atomic.h>
#include <Adafruit_SleepyDog.h>

volatile int revolutions;
int revValue;
float rpm;
int rmpMap_r = 255;
int rmpMap_g = 10;
int rmpMap_b = 0;
unsigned long threshold;
unsigned long timerStart;
unsigned long timerAmbientStart;
int ambientElapsed = 5000;

// NEO PIXELS
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN 6
#define LED_COUNT 32
#define BRIGHTNESS 255  // Set BRIGHTNESS to about 1/5 (max = 255)
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);

// MAPPING
uint8_t start_r = 255;
uint8_t start_g = 10;
uint8_t start_b = 0;
uint8_t end_r = 0;
uint8_t end_g = 0;
uint8_t end_b = 0;


// LED MINS AND MAXES
long maxRPM = 420;

void setup() {
  // Setup hall effect
  Serial.begin(9600);
  attachInterrupt(0, magnet_detect, RISING);  //Initialize the intterrupt pin (Arduino digital pin 2)
  revolutions = 0;
  rpm = 0;
  timerStart = 0;
  threshold = 1000;  // 1 second

  // Setup neopixels
  strip.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();   // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);

  // enable watchdog
  int countdownMS = Watchdog.enable(30000);  // 10 seconds
}

bool startTimer = false;

void loop() {

  // HALL EFFECT DATA
  updateHallEffect();

  // MAP RMP
  //Serial.println("rmpMap: " + String(rmpMap));

  // NEO PIXEL ANIMATION
  // WIP: Ambient Animation
  // start time

  if (rpm == 0.00 && !startTimer) {
    startTimer = true;
    timerAmbientStart = millis();
  } else if (rpm > 0) {
    startTimer = false;
  }

  unsigned long elapsed_time = millis() - timerAmbientStart;

  if (startTimer && elapsed_time > ambientElapsed) {
    Serial.println("ANIMATING BRIGHTNESS " + (String)startTimer);
    // brightness animation
    animate_gradient_fill(255, 10, 0, 255, 100, 0, 0, 255, 10000);
    animate_gradient_fill(100, 0, 0, 255, 255, 10, 0, 255, 10000);
  } else {
    Serial.println("ANIMATING SENSOR: " + (String)elapsed_time);
    int middleThreshold = 220;
    int maxThreshold = 420;

    rmpMap_r = map(rpm, 0, middleThreshold, 0, 255);
    rmpMap_r = constrain(rmpMap_r, 0, 255);

    rmpMap_g = map(rpm, 0, middleThreshold, 0, 10);
    rmpMap_g = constrain(rmpMap_g, 0, 10);

    rmpMap_b = map(rpm, middleThreshold, maxThreshold, 0, 255);
    rmpMap_b = constrain(rmpMap_b, 0, 255);

    end_r = 255 - rmpMap_r;
    end_g = 10 - rmpMap_g;
    end_b = rmpMap_b;
    // animate_gradient_fill(start_r, 0, start_b, 255, end_r, 0, end_b, 255, 1000);
    animate_gradient_fill(start_r, start_g, start_b, 255, end_r, end_g, end_b, 255, 1000);
    start_r = end_r;
    start_g = end_g;
    start_b = end_b;
  }


  // Reset watchdog
  Watchdog.reset();
}

// HALL EFFECT
void updateHallEffect() {

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    revValue = revolutions;
  }

  unsigned long difference = millis() - timerStart;

  if (difference >= threshold) {
    //Serial.println("revValue: " + (String)revValue);

    // 1 rev / 1 ms X 1000 ms / 1 s X 60 s / 1 min
    rpm = 1000.0 * 60.0 / (float)threshold * (float)revValue;
    timerStart = millis();
    revolutions = 0;
    Serial.println("rpm, starttimer: " + (String)rpm + ", " + (String)startTimer);
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

  if (startTimer && rpm > 0) {
    start_r = red;
    start_g = green;
    start_b = blue;
    return;
  }

  return Adafruit_NeoPixel::Color(red, green, blue, white);
}

float lerp(float x, float x0, float x1, float y0, float y1) {
  x = x > x1 ? x1 : x;
  x = x < x0 ? x0 : x;
  return y0 + (y1 - y0) * ((x - x0) / (x1 - x0));
}