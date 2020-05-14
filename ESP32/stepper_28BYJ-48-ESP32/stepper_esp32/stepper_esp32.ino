/* Example sketch to control a 28BYJ-48 stepper motor with ULN2003 driver board and ESP32
 *  
 */
// Include the Arduino Stepper.h library:
#include <Stepper.h>

// Define number of steps per rotation:
const int stepsPerRevolution = 2048;

// Wiring ESP32:
// Pin 33 to IN1 on the ULN2003 driver
// Pin 15 to IN2 on the ULN2003 driver
// Pin 32 to IN3 on the ULN2003 driver
// Pin 14 to IN4 on the ULN2003 driver

// Create stepper object called 'myStepper', note the pin order:
Stepper myStepper = Stepper(stepsPerRevolution, 33, 32, 15, 14);

void setup() {
  // Set the speed to 5 rpm:
  myStepper.setSpeed(7);
  // Begin Serial communication at a baud rate of 9600:
  Serial.begin(115200);
}

void loop() {
  // Step one revolution in one direction:
  Serial.println("clockwise");
  myStepper.step(stepsPerRevolution);
  delay(500);
  
  // Step one revolution in the other direction:
  Serial.println("counterclockwise");
  myStepper.step(-stepsPerRevolution);
  delay(500);
}
