#include <Wire.h>
#include "Adafruit_DRV2605.h"
#include <ESP32Encoder.h>

Adafruit_DRV2605 drv;
int sensorValue = 0;  // variable to store the value coming from the sensor

int8_t curMode;
ESP32Encoder encoder;
int32_t encoderLastCount = -9999;
int32_t encoderCount = encoder.getCount();

void setup() {
  // hall sensor pin
  pinMode(27, INPUT_PULLUP); // btn
  Serial.begin(115200);
  delay(500);
  Serial.println("DRV test");
  drv.begin();

  drv.selectLibrary(1);

  // I2C trigger by sending 'go' command
  // default, internal trigger when sending GO command
  //
  drv.useERM();
  playHapticRT(0.0);
  //drv.setMode(1);

  // ENCODER STUFF
  // Enable the weak pull down resistors

  //ESP32Encoder::useInternalWeakPullResistors=DOWN;
  // Enable the weak pull up resistors
  ESP32Encoder::useInternalWeakPullResistors = UP;

  // use pin 19 and 18 for the first encoder
  encoder.attachHalfQuad(13, 15);
  encoder.setFilter(1023);
  // set starting count value after attaching
  encoder.setCount(0);

  Serial.println("Encoder Start = " + String((int32_t)encoder.getCount()));


}

//uint8_t effect = 17;


int32_t encoderCountPrev = 0;


void loop() {

  //  Serial.println(digitalRead(27));
  if (digitalRead(27) == LOW) {
    setMode(DRV2605_MODE_INTTRIG);
    playHaptic(1);
    delay(500);
  }

  encoderCount = encoder.getCount();
  //Serial.println(encoderCount);
  if (encoderCount != encoderCountPrev)
  {
    int32_t delta = encoderCount - encoderCountPrev;
    Serial.println(delta);

    // send delta
  }
  double motorInput = (float) encoderCount/10;
  //playHapticRT(motorInput);

  // event: realtime
  /*setMode(DRV2605_MODE_REALTIME);
    double motorInput = 0.5;
    playHapticRT(motorInput);*/

  // event: trigger

  if (encoderCount != encoderLastCount) {
    Serial.println("Encoder count = " + String((int32_t)encoderCount));
    encoderLastCount = encoderCount;
    if ((encoderCount % 10) == 1) {
      Serial.println("bzz");
      playHaptic(59);
    }
  }

  encoderCountPrev = encoderCount;
}

void setMode(uint8_t mode) {
  if (mode == curMode)
    return;
  drv.setMode(mode);
  curMode = mode;
}
void playHaptic(uint8_t effect) {
  setMode(DRV2605_MODE_INTTRIG);

  drv.setWaveform(0, effect);  // play effect
  drv.setWaveform(1, 0);       // end waveform
  drv.go();
  //delay(200);
}

void playHapticRT(double val) { // 0 - 1
  setMode(DRV2605_MODE_REALTIME);

  int intV = min(1.0, max(0.0, val)) * 0x7F;
  //Serial.println(intV);
  drv.setRealtimeValue(intV);
}
