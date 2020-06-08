// ESP32 Touch Test
// Just test touch pin - Touch0 is T0 which is on GPIO 4.

long start_touch;
long time_touched;
bool touch = false;



void setup()
{
  Serial.begin(115200);
  delay(1000); // give me time to bring up serial monitor
  Serial.println("ESP32 Touch Test");
}

void loop()
{
  //Serial.println(touchRead(T0));  // get value using T0
  int capacitive = touchRead(T0);

  if (capacitive < 70 && touch == false) {
    start_touch = millis();
    touch = true;
    Serial.println("- - > TOUCH DOWN");
  } else if (capacitive > 70 && touch == true) {
    time_touched = millis() - start_touch;
    if (time_touched < 1000 ) {
      Serial.println("- - > TOUCH UP");
    } else {
      Serial.println("- - > TOUCH UP (Long Touch)");
    }
    touch = false;
  }
}
