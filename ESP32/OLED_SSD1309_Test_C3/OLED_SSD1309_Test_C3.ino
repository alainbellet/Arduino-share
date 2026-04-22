/*
  OLED SSD1309 Test for LOLIN C3 Pico

  Wiring recommendation:
  OLED     Lolin C3 Pico 
  -----    -------------
  VCC  ->  3.3V (e.g. bottom right)
  GND  ->  GND  (e.g. above 3V3)
  SCK  ->  4    (Left side, 4th from top)
  SDA  ->  6    (Right side, 3rd from bottom)
  CS   ->  7    (Right side, 4th from bottom)
  DC   ->  8    (Right side, 5th from bottom)
  RES  ->  10   (Right side, 7th from bottom)

*/

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>

// Initialize U8g2 for SSD1309 128x64 display over 4-wire SPI.
// We use software SPI so we can map it to any available pins.
// Parameters: rotation, clock (SCK), data (SDA), cs, dc, reset
U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 4, /* data=*/ 6, /* cs=*/ 7, /* dc=*/ 8, /* reset=*/ 10);

void setup() {
  Serial.begin(115200);
  
  // Short delay to allow serial to initialize
  delay(1000); 
  Serial.println("Starting OLED SSD1309 test...");
  
  // Initialize the display
  u8g2.begin();
}

void loop() {
  // Clear internal memory
  u8g2.clearBuffer();
  // Create a negative (inverted) effect
  // First, fill the entire screen with white (color 1)
  u8g2.setDrawColor(1);
  u8g2.drawBox(0, 0, 128, 64);
  
  // Now set the drawing color to black (color 0) to "carve out" pixels
  u8g2.setDrawColor(0);
  
  // Draw an inner frame with our black color
  u8g2.drawFrame(1, 1, 126, 62);
  
  // Draw title text
  u8g2.setFont(u8g2_font_micro_tr); // Tiny font (3x5 pixels)
  u8g2.drawStr(5, 10, "C3 Pico Test (Smallest Font)");
  
  // Draw changing text (uptime)
  u8g2.setFont(u8g2_font_micro_tr); // Tiny font
  u8g2.setCursor(5, 20);
  u8g2.print("Uptime: ");
  u8g2.print(millis() / 1000);
  u8g2.print(" seconds");
  
  // Transfer internal memory to the display
  u8g2.sendBuffer();
  
  delay(100);
}
