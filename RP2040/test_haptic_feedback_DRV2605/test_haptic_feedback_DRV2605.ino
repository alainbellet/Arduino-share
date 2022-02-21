#include <Wire.h>
#include "Adafruit_DRV2605.h"

Adafruit_DRV2605 drv;

void setup() {
  Serial.begin(9600);
  Serial.println("DRV test");
  drv.begin();

  drv.selectLibrary(1);

  // I2C trigger by sending 'go' command
  // default, internal trigger when sending GO command
  drv.setMode(DRV2605_MODE_INTTRIG);
  Serial.println("ERM: (Eccentric Rotating Mass) mode active"); // Default

  Serial.println("which effect? (1-123)");        //Prompt User for input
  Serial.println("ERM or LRA mode? (ERM type 200; LRA type 300)");        //Prompt User for input
  //drv.useERM(); //Use ERM (Eccentric Rotating Mass) mode.
  drv.useLRA(); //Use LRA (Linear Resonance Actuator) mode.
}

uint8_t effect = 1;


void loop() {
  int cmd = 0;
  while (Serial.available() > 0)
  {
    cmd = Serial.parseInt();
    if (cmd != 0) {
      effect = cmd;
    }
    Serial.println("Effect " + String(effect) + " triggered");
    // set the effect to play
    drv.setWaveform(0, effect);  // play effect
    drv.setWaveform(1, 0);       // end waveform

    // play the effect!
    drv.go();
  }
}
