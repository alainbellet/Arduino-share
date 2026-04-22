/*
  I2C OLED Test (SSD1306/SH1106) for LOLIN C3 Pico

  Wiring recommendation:
  OLED     Lolin C3 Pico
  -----    -------------
  GND  ->  GND  (e.g. Right side, 2nd from bottom)
  VCC  ->  3.3V (e.g. Right side, bottom pin)
  SCL  ->  7    (Right side, 4th from bottom)
  SDA  ->  6    (Right side, 3rd from bottom)

*/

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// Initialize U8g2 for SSD1306 128x64 display over Software I2C.
// We use software I2C (SW_I2C) so we can cleanly map SCL and SDA to pins 7
// and 6. Parameters: rotation, clock (SCL), data (SDA), reset (NONE)
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/7, /* data=*/6,
                                         /* reset=*/U8X8_PIN_NONE);

// Note: If the text is shifted horizontally by a couple of pixels and wraps
// around to the other side, your display might actually use the SH1106
// controller instead of the SSD1306. If that happens, comment out the line
// above and uncomment the line below: U8G2_SH1106_128X64_NONAME_F_SW_I2C
// u8g2(U8G2_R0, /* clock=*/ 7, /* data=*/ 6, /* reset=*/ U8X8_PIN_NONE);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting I2C OLED test...");

  u8g2.begin();
}

void loop() {
  u8g2.clearBuffer();

  // Draw a border
  u8g2.drawFrame(0, 0, 128, 64);

  // Draw Title
  u8g2.setFont(u8g2_font_ncenB12_tr);
  u8g2.drawStr(12, 14, "I2C OLED");

  // Draw Uptime text
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(20, 30);
  u8g2.print("Uptime: ");
  u8g2.print(millis() / 1000);
  u8g2.print("s");

  u8g2.sendBuffer();

  delay(100);
}
