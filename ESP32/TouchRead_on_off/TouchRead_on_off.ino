// ESP32 Touch Test
// Just test touch pin - Touch0 is T0 which is on GPIO 4.

bool touch_state = 0;
bool last_touch_state = 0;

void setup()
{
  Serial.begin(115200);
  delay(1000); // give me time to bring up serial monitor
  Serial.println("ESP32 Touch Test");
}

void loop()
{
  int touch_input = touchRead(T2);
 // Serial.println(touch_input);

  if (touch_input < 10) {
    touch_state = true;
  } else if (touch_input > 50) {
    touch_state = false;
  }

  if (last_touch_state != touch_state) {
    if (touch_state == true) {
      Serial.print("Touch ");
      Serial.println(touch_input);
    } else if (touch_state == false) {
      Serial.print("No Touch");
      Serial.println(touch_input);
    }
    last_touch_state = touch_state;
    delay(300);
  }



}
