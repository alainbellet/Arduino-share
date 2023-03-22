#include "WiFiNINA.h"  // needed for LED pins

void setup() {
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
}

void set_rgb_led(int r, int g, int b) {
  digitalWrite(LEDR, r == 0 ? LOW : HIGH);
  digitalWrite(LEDG, g == 0 ? LOW : HIGH);
  digitalWrite(LEDB, b == 0 ? LOW : HIGH);
}

void loop() {
  set_rgb_led(1, 0, 0);  // red
  delay(2000);
  set_rgb_led(0, 1, 0);  // green
  delay(2000);
  set_rgb_led(0, 0, 1);  // blue
  delay(2000);
  set_rgb_led(1, 1, 0);  // yellow
  delay(2000);
  set_rgb_led(0, 1, 1);  // cyan
  delay(2000);
  set_rgb_led(1, 0, 1);  // purple
  delay(2000);
  set_rgb_led(0, 0, 0);  // off/black
  delay(2000);
  set_rgb_led(1, 1, 1);  // white
  delay(2000);
}
