/* 
A. Bellet 2023
*/
#include "Mouse.h"
// pin used for the two buttons on the Pi Pico
// one side of the button goes to pin, the other side goes to GND
int pin_mouse_left = 1;
int pin_mouse_right = 2;
int mouse_state_left = 1;
int mouse_state_right = 1;
int last_mouse_state_left = 1;
int last_mouse_state_right = 1;

void setup() {
  // initialize buttons pins
  pinMode(pin_mouse_left, INPUT_PULLUP);
  pinMode(pin_mouse_right, INPUT_PULLUP);
  // initialize mouse control:
  Mouse.begin();
}

void loop() {
  // check button states
  mouse_state_left = digitalRead(pin_mouse_left);
  mouse_state_right = digitalRead(pin_mouse_right);
  // to test without buttons
  bool test_without_button = true;  // set to false to use with real buttons
  if (test_without_button == true) {
    // switch every 1 seconde
    if (Mouse.isPressed(MOUSE_LEFT)) {
      Mouse.release(MOUSE_LEFT);
      Mouse.press(MOUSE_RIGHT);
    } else {
      Mouse.release(MOUSE_RIGHT);
      Mouse.press(MOUSE_LEFT);
    }
    delay(1000);
    return;  // stop execution here
  }

  // Left
  if (mouse_state_left != last_mouse_state_left) {  // check if state has changed
    if (digitalRead(pin_mouse_left) == LOW) {
      Mouse.press(MOUSE_LEFT);
    }
    if (digitalRead(pin_mouse_left) == HIGH) {
      Mouse.release(MOUSE_LEFT);
    }
    last_mouse_state_left = mouse_state_left;
  }
  // Right
  if (mouse_state_right != last_mouse_state_right) {  // check if state has changed
    if (digitalRead(pin_mouse_right) == LOW) {
      Mouse.press(MOUSE_RIGHT);
    }
    if (digitalRead(pin_mouse_right) == HIGH) {
      Mouse.release(MOUSE_RIGHT);
    }
    last_mouse_state_right = mouse_state_right;
  }
  delay(200);
}
