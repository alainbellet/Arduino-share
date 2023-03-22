int pot1;
int potmap;

void setup() {
  Serial.begin(115200);
  pinMode(A0, INPUT);
}

void loop() {
  pot1 = analogRead(A0);
  potmap = map(pot1, 6, 1022, 0, 100);
  Serial.println(potmap);
}
