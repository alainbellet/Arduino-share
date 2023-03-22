#include "LSM6DSOXSensor.h"

#define INT_1 INT_IMU
volatile int mems_event = 0;
LSM6DSOXSensor AccGyr(&Wire, LSM6DSOX_I2C_ADD_L);

int tapCounter = 0;

void INT1Event_cb() {
  mems_event = 1;
}

void setup() {
  Serial.begin(115200);
  // Initialize I2C bus.
  Wire.begin();
  Wire.setClock(400000);

  //Interrupts.
  attachInterrupt(INT_1, INT1Event_cb, RISING);

  // Initialize IMU.
  AccGyr.begin();
  AccGyr.Enable_X();
  AccGyr.Enable_G();

  // Enable the Sinle Tap event
  AccGyr.Enable_Single_Tap_Detection(LSM6DSOX_INT1_PIN);
}

void loop() {
  if (mems_event) {
    mems_event = 0;
    LSM6DSOX_Event_Status_t status;
    AccGyr.Get_X_Event_Status(&status);

    if (status.TapStatus) {
      Serial.println("Single Tap Detected!");
      Serial.println(tapCounter );
      tapCounter++;
    }
  }
}
