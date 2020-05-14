/* 
 *  Example sketch to control a 28BYJ-48 stepper motor with ULN2003 driver board, AccelStepper and ESP32 
 *  
 */

// Include the AccelStepper library:
#include <AccelStepper.h>

// Motor pin definitions:
#define motorPin1  33      // IN1 on the ULN2003 driver
#define motorPin2  15      // IN2 on the ULN2003 driver
#define motorPin3  32     // IN3 on the ULN2003 driver
#define motorPin4  14     // IN4 on the ULN2003 driver
// GND to GND
// USB PIN (5V) to VCC (or BAT if battery powered but might be to low voltage to run properely)


// Define the AccelStepper interface type; 4 wire motor in half step mode:
#define MotorInterfaceType 8

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper library with 28BYJ-48 stepper motor:
AccelStepper stepper = AccelStepper(MotorInterfaceType, motorPin1, motorPin3, motorPin2, motorPin4);

void setup() {
  // Set the maximum steps per second:
  stepper.setMaxSpeed(1000);
  // Set the maximum acceleration in steps per second^2:
  stepper.setAcceleration(200);
}

void loop() {
  // Set target position:
  stepper.moveTo(8192);
  // Run to position with set speed and acceleration:
  stepper.runToPosition();
  
  delay(1000);
  
  // Move back to original position:
  stepper.moveTo(0);
  // Run to position with set speed and acceleration:
  stepper.runToPosition();
  
  delay(1000);
}
