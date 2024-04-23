
#define BUTTON_PIN 13

int last_btn_state = 1;


void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  delay(100);
}

void loop() {
  int btn_state = digitalRead(BUTTON_PIN);
  if (last_btn_state != btn_state) {  // only pass if state changed
    if (btn_state == LOW) { 
      Serial.println("Button pressed");
    }
    if (btn_state == HIGH) {
      Serial.println("Button released");
    }
    last_btn_state = btn_state;
    delay(30);
  }
}
