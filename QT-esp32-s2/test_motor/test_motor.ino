#include <Wire.h>
#include "Adafruit_DRV2605.h"
Adafruit_DRV2605 drv;

const int motorPin = A3;  // 16 corresponds to GPIO16

// setting PWM properties
const int freq = 1000;
const int pwmChannel = 0;
const int resolution = 8;

void setup() {
  Serial.begin(115200);
  delay(1000);
  if (!drv.begin(&Wire1)) {
    Serial.println("Could not find DRV2605");
    while (1) delay(10);
  }
  pinMode(A3, OUTPUT);
  // configure MOTOR PWM functionalitites
  ledcSetup(pwmChannel, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(motorPin, pwmChannel);
  drv.selectLibrary(1);
  drv.useERM();
}

void loop() {
  //digitalWrite(motorPin, 1);
  ledcWrite(pwmChannel, 200);
  // set the effect to play
  drv.setWaveform(0, 17);  // play effect
  drv.setWaveform(1, 0);   // end waveform
  // play the effect!
  drv.go();
  delay(2000);
  ledcWrite(pwmChannel, 150);
  drv.go();
  delay(2000);
  ledcWrite(pwmChannel, 100);
  drv.go();
  delay(2000);
  drv.go();
  ledcWrite(pwmChannel, 0);
  delay(2000);
  drv.go();
  ledcWrite(pwmChannel, 0);
  delay(2000);
  //digitalWrite(motorPin, 0);
  //delay(2000);
}
