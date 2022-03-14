/*---------------------------------------------------------------------------------------------

  Open Sound Control (OSC) library for the ESP8266/ESP32

  Example for sending messages from the ESP8266/ESP32 to a remote computer
  The example is sending "hello, osc." to the address "/test".

  This example code is in the public domain.

  --------------------------------------------------------------------------------------------- */
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include <OSCMessage.h>

char ssid[] = "sstudios";          // your network SSID (name)
char pass[] = "quickbrownhunz";                    // your network password

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(192, 168, 10, 56);     // remote IP of your computer
const unsigned int outPort = 8888;          // remote port to receive OSC
const unsigned int localPort = 9999;        // local port to listen for OSC packets (actually not used for sending)

// Button
const int buttonPin = 21;
int buttonState = 0;
int buttonLastState = -1;
// potentiometer
const int potentiometerPin = 34;
int potentiometerState = 0;
int potentiometerLastState = -1;

void setup() {
  Serial.begin(115200);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
#ifdef ESP32
  Serial.println(localPort);
#else
  Serial.println(Udp.localPort());
#endif

}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  // read the state of the potentiometer
  potentiometerState = map(analogRead(potentiometerPin), 0, 4096, 0, 100) ;
  if (potentiometerState != potentiometerLastState) {
    potentiometerLastState = potentiometerState;
    OSCMessage msg("/unity/potentiometer/");
    msg.add(potentiometerState);
    Udp.beginPacket(outIp, outPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();

    Serial.println(potentiometerState);
    delay(10);
  }
  if (buttonState != buttonLastState) {
    buttonLastState = buttonState;
    OSCMessage msg("/unity/button/");
    msg.add(buttonState);
    Udp.beginPacket(outIp, outPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();

    Serial.println(buttonState);
    delay(10);
  }
}
