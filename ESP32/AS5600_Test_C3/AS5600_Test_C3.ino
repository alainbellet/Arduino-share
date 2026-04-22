/*
  AS5600 Magnetic Encoder Test for LOLIN C3 Pico

  Wiring recommendation:
  AS5600   Lolin C3 Pico 
  -----    -------------
  GND  ->  GND  (e.g. Right side, 2nd from bottom)
  VCC  ->  3.3V (e.g. Right side, bottom pin)
  SCL  ->  7    (Right side, 4th from bottom)
  SDA  ->  6    (Right side, 3rd from bottom)
  DIR  ->  GND  (Sets clockwise/counter-clockwise direction, optional for simple tests)

  Note: The AS5600 I2C address is fixed at 0x36.
*/

#include <Wire.h>

#define AS5600_ADDRESS 0x36
#define RAW_ANGLE_REGISTER 0x0C

// We map hardware I2C to pins 6 & 7 (same as you used before for convenience!)
#define I2C_SDA 6
#define I2C_SCL 7

void setup() {
  Serial.begin(115200);
  
  // Short delay to allow serial to spin up
  delay(1500); 
  Serial.println("\n--- Starting AS5600 Encoder Test ---");

  // Initialize I2C with our specific pins
  Wire.begin(I2C_SDA, I2C_SCL);

  // Check if AS5600 is connected over I2C
  Wire.beginTransmission(AS5600_ADDRESS);
  byte error = Wire.endTransmission();
  
  if (error == 0) {
    Serial.println("SUCCESS: AS5600 found at address 0x36!");
  } else {
    Serial.println("ERROR: AS5600 not found! Please check your wiring.");
  }
  
  Serial.println("------------------------------------");
}

void loop() {
  // Tell the AS5600 we want to read starting from the RAW_ANGLE register (0x0C)
  Wire.beginTransmission(AS5600_ADDRESS);
  Wire.write(RAW_ANGLE_REGISTER);
  Wire.endTransmission();

  // Request 2 bytes (High byte and Low byte of the 12-bit angle)
  Wire.requestFrom(AS5600_ADDRESS, 2);
  
  if (Wire.available() >= 2) {
    byte highByte = Wire.read();
    byte lowByte = Wire.read();
    
    // Combine the two bytes into a 12-bit value (0 - 4095)
    uint16_t rawAngle = (highByte << 8) | lowByte;
    
    // Calculate the angle in degrees (0.0 to 359.9)
    float degrees = (rawAngle / 4096.0) * 360.0;
    
    // Print data for Serial Monitor
    Serial.print("Raw: ");
    Serial.print(rawAngle);          // The raw 0-4095 value
    Serial.print(" \t | \t Degrees: ");
    Serial.println(degrees, 1);      // The angle to 1 decimal place
    
    // Hint: Open the Serial Plotter (Tools -> Serial Plotter) to see this visually!
  }

  // A small delay to not overwhelm the serial output
  delay(100); 
}
