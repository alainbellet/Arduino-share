#include <Wire.h>
#include "Adafruit_DRV2605.h"

Adafruit_DRV2605 drv;
int sensorValue = 0;  // variable to store the value coming from the sensor

void setup() {
  Serial.begin(9600);
  Serial.println("DRV test");
  drv.begin();

  drv.selectLibrary(1);

  // I2C trigger by sending 'go' command
  // default, internal trigger when sending GO command
  drv.setMode(DRV2605_MODE_INTTRIG);
  drv.useLRA();
  //drv.setMode(1);
  
}

uint8_t effect = 17;

void loop() {
  sensorValue = analogRead(A0);
  if (sensorValue % 100 == 1) {

    //Serial.println("which effect?");        //Prompt User for input
    //while (Serial.available() == 0) {}          // wait for user input
    //effect = Serial.parseInt();

    // set the effect to play
    drv.setWaveform(0, effect);  // play effect
    drv.setWaveform(1, 0);       // end waveform

    // play the effect!
    drv.go();
    // wait a bit
    delay(200);
  }


}
