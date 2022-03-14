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
#include <OSCBundle.h>
#include <OSCData.h>

//char ssid[] = "sstudios";          // your network SSID (name)
//char pass[] = "quickbrownhunz";                    // your network password
char ssid[] = "asdf";          // your network SSID (name)
char pass[] = "1loveyou";                    // your network password

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(192, 168, 45, 79);    //63 // remote IP of your computer
const unsigned int outPort = 8888;          // remote port to receive OSC
const unsigned int localPort = 9999;        // local port to listen for OSC packets (actually not used for sending)

OSCErrorCode error;

// Button
const int buttonPin = 21;
int buttonState = 0;
int buttonLastState = -1;
// potentiometer
const int potentiometerPin = 34;
int potentiometerState = 0;
int potentiometerLastState = -1;
// LED
unsigned int ledState = LOW;

void setup() {
  Serial.begin(115200);
  analogReadResolution(9); // 0-511
  //analogSetAttenuation(ADC_0db);

  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  // initialize the LED pin as an output:
  pinMode(LED_BUILTIN, OUTPUT);


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

void led(OSCMessage &msg) {
  ledState = msg.getInt(0);
  digitalWrite(BUILTIN_LED, ledState);
  Serial.print("/arduino/led: ");
  Serial.println(ledState);
}

void sendValues() {
  OSCMessage msg("/unity/state/");
  msg.add(buttonState);
  msg.add(potentiometerState);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
  delay (10);

}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  // read the state of the potentiometer
  potentiometerState = analogRead(potentiometerPin);
  potentiometerState = map(potentiometerState, 0, 511, 0, 100) ;

  if (potentiometerState != potentiometerLastState) {
    sendValues();
    potentiometerLastState = potentiometerState;
    /*
    OSCMessage msg("/unity/potentiometer/");
    msg.add(potentiometerState);
    Udp.beginPacket(outIp, outPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();

    Serial.println(potentiometerState);
    delay(10);*/
  }
  if (buttonState != buttonLastState) {
    sendValues();
    buttonLastState = buttonState;
    /*
    OSCMessage msg("/unity/button/");
    msg.add(buttonState);
    Udp.beginPacket(outIp, outPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();

    Serial.println(buttonState);
    delay(10);
    */
  }

  // IN
  OSCMessage msg;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      msg.fill(Udp.read());
    }
    Serial.println("msg");
    if (!msg.hasError()) {
      msg.dispatch("/arduino/led", led);
    } else {
      error = msg.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }
}
