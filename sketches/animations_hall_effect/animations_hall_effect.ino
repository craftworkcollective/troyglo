//HALL EFFECT SENSOR
#include <util/atomic.h>
volatile int revolutions;
int revValue;
float rpm;
unsigned long threshold;
unsigned long timerStart;



#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN     6
#define LED_COUNT  16
#define BRIGHTNESS 50 // Set BRIGHTNESS to about 1/5 (max = 255)
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);

void setup() {
  // Setup hall effect
  Serial.begin(9600);
  attachInterrupt(0, magnet_detect, RISING);//Initialize the intterrupt pin (Arduino digital pin 2)
  revolutions = 0;
  rpm = 0;
  timerStart = 0;
  threshold = 1000; // 10 seconds

  // Setup neopixels
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);
}

void loop() {

  updateHallEffect();

  int mappedVal = map(rpm, 0, 300, 0, 255);

  Serial.print("mappedVal: ");
  Serial.println(mappedVal);

  strip.fill(strip.Color(0, 0, mappedVal, strip.gamma8(255)));
  strip.show();

}

// HALL EFFECT
void updateHallEffect()
{

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    revValue = revolutions;
  }

  unsigned long difference = millis() - timerStart;

  if (difference >= threshold) {

    Serial.print("revValue: ");
    Serial.println(revValue);

    // 1 rev / 1 ms X 1000 ms / 1 s X 60 s / 1 min
    rpm = 1000.0 * 60.0 / (float) threshold * (float) revValue;
    timerStart = millis();
    revolutions = 0;

    Serial.print("rpm: ");
    Serial.println(rpm);

  }
}

void magnet_detect()//This function is called whenever a magnet/interrupt is detected by the arduino
{
  revolutions++;
  Serial.print(revolutions);
  Serial.println(" detect");
}

// ANIMATIONS
void blueYellow()
{
  strip.fill(strip.Color(255, 255, 224, strip.gamma8(255)));
  strip.show();
}

void blueWhite()
{
  strip.fill(strip.Color(0, 0, 255, strip.gamma8(255)));
  strip.show();
}

void fullWhite()
{
  strip.fill(strip.Color(0, 0, 0, strip.gamma8(255)));
  strip.show();
}

void pulseWhite(uint8_t wait) {
  for (int j = 0; j < 256; j++) { // Ramp up from 0 to 255
    // Fill entire strip with white at gamma-corrected brightness level 'j':
    strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
    strip.show();
    delay(wait);
  }

  for (int j = 255; j >= 0; j--) { // Ramp down from 255 to 0
    strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
    strip.show();
    delay(wait);
  }
}
