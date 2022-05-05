/* comment the second part to deactivate
#define Sprint(a) // (Serial.print(a))
*/
#define Sprint(a) (Serial.print(a))
#define Sprintln(a) (Serial.println(a))
#define Sbegin(a) (Serial.begin(a))

void setup() {
  Sbegin(9600);
}

void loop() {
  Sprintln("yo");
}
