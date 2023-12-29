#include <Wire.h>
#include "Adafruit_DRV2605.h"
Adafruit_DRV2605 drv;
const int motorPin = A3;  // 16 corresponds to GPIO16

// setting PWM properties
const int freq = 1000;
const int pwmChannel = 0;
const int resolution = 8;

void setup() {
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
  drv.setMode(DRV2605_MODE_INTTRIG);
}

void loop() {
  // increase the LED brightness
  for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle--) {
    // changing the LED brightness with PWM
    ledcWrite(pwmChannel, dutyCycle);
    delay(10);
  }
  delay(500);
  for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle++) {
    // changing the LED brightness with PWM
    ledcWrite(pwmChannel, dutyCycle);
    delay(10);
  }
  ledcWrite(pwmChannel, 255);
  delay(2000);

  // ledcWrite(pwmChannel, 255);
  // delay(10);
  // ledcWrite(pwmChannel, 50);
  // delay(10);
  // ledcWrite(pwmChannel, 0);
}
