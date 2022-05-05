#include <ESP32Encoder.h>

ESP32Encoder encoder;

int32_t encoderLastCount = -9999;
int32_t encoderCount = encoder.getCount();
void setup() {

  Serial.begin(115200);
  // Enable the weak pull down resistors

  //ESP32Encoder::useInternalWeakPullResistors=DOWN;
  // Enable the weak pull up resistors
  ESP32Encoder::useInternalWeakPullResistors = UP;

  // use pin 19 and 18 for the first encoder
  encoder.attachHalfQuad(19, 23);
  encoder.setFilter(1023);
  // set starting count value after attaching
  encoder.setCount(0);

  Serial.println("Encoder Start = " + String((int32_t)encoder.getCount()));
}

void loop() {
  // Loop and read the count
  int32_t encoderCount = encoder.getCount();
  if (encoderCount != encoderLastCount) {
    Serial.println("Encoder count = " + String((int32_t)encoderCount));
    encoderLastCount = encoderCount;
    delay(10);
  }

}
