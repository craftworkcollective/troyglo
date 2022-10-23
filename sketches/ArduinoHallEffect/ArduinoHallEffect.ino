#include <util/atomic.h>
volatile int revolutions;
int revValue; 
 float rpm;
unsigned long threshold;
unsigned long timerStart;

void setup()
{
  Serial.begin(9600);
  attachInterrupt(0, magnet_detect, RISING);//Initialize the intterrupt pin (Arduino digital pin 2)
  revolutions = 0;
  rpm = 0;
  timerStart = 0;
  threshold = 10000; // 10 seconds
}
void loop()
{
  updateHallEffect(); 
}

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
