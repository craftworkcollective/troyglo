//HALL EFFECT SENSOR
#include <util/atomic.h>
volatile int revolutions;
int revValue;
float rpm;
int rmpMap;
unsigned long threshold;
unsigned long timerStart;

// AVERAGING HALL DATA
const int numTimestamps = 50;
unsigned long timeStamps[numTimestamps];  // array of timstamps
int tsIndex, startIndex, endIndex = 0;                          //index of current time stamp
int total = 0;
unsigned long average = 0;


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

  // setup time stamps
  for (int index = 0; index < numTimestamps; index++) {
    timeStamps[index] = 0;
  }

  // Setup neopixels
  strip.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();   // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);
}

void loop() {

  // HALL EFFECT DATA
  updateHallEffect();

  // MAP RMP
  rmpMap = map(rpm, 0, 420, 0, 255);
  rmpMap = constrain(rmpMap, 0, 255);
  //Serial.print("rmpMap: ");
  //Serial.println(rmpMap);

  // NEO PIXEL ANIMATION
  strip.fill(strip.Color(0, 0, rmpMap, strip.gamma8(255)));
  strip.show();
}

// HALL EFFECT
void updateHallEffect() {

  int localEndIndex; 
  unsigned long localTimeStamps[numTimestamps]; 
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    revValue = revolutions;
    localEndIndex = endIndex; 

    for (int index = 0; index < numTimestamps; index++) {
       localTimeStamps[index] = timeStamps[index]; 
    }
  }

  unsigned long difference = millis() - timerStart;

  if (difference >= threshold) {
    Serial.println("revValue: " + (String)revValue);

    /*
    for (int index = 0; index < numTimestamps; index++) {
      Serial.println((String)index + ": " + (String)localTimeStamps[index]);
    }
    */ 

    // 1 rev / 1 ms X 1000 ms / 1 s X 60 s / 1 min
    rpm = 1000.0 * 60.0 / (float)threshold * (float)revValue;
    timerStart = millis();
    revolutions = 0;
    Serial.println("rpm: " + (String)rpm);

    // reset startindex
    startIndex = localEndIndex; 
  }
}

void magnet_detect()  //This function is called whenever a magnet/interrupt is detected by the arduino
{
  revolutions++;
  //Serial.println(String(revolutions) + " detect");


  timeStamps[tsIndex] = millis();
  endIndex = tsIndex; 
  tsIndex++; 

  // reset index
  if (tsIndex >= numTimestamps) {
    tsIndex = 0;
  }
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
