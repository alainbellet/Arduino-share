#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <ESP32Servo.h>
static const int servoPin = 2;

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */

Servo myservo;  // create servo object to control a servo

void setup(void) 
{
  Serial.begin(9600);
  Serial.println("Hello!");
  
  Serial.println("Getting single-ended readings from AIN0..3");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");
  ads.begin();

  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 500, 2400); // attaches the servo on pin 18 to the servo object
  // using default min/max of 1000us and 2000us
  // different servos may require different min/max settings
  // for an accurate 0 to 180 sweep
}

void loop(void) 
{
  int16_t adc0, adc1, adc2, adc3;

  adc0 = ads.readADC_SingleEnded(0);
 
  
  //Serial.print("AIN0: "); Serial.println(adc0);
  adc0 = map(adc0, 0, 17560, 5, 180);
  Serial.print("AIN0 (mapped): "); Serial.println(adc0);
  myservo.write(adc0);
  
  delay(50);
}
