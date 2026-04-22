

#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <Adafruit_NeoPixel.h>

char ssid[] = "***";    // your network SSID (name)
char pass[] = "***";  // your network password

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
const IPAddress outIp(172, 20, 10, 2);  // remote IP (not needed for receive)
const unsigned int outPort = 8888;       // remote port (not needed for receive)
const unsigned int localPort = 8888;     // local port to listen for UDP packets (here's where we send the packets)


OSCErrorCode error;
unsigned int ledState = LOW;  // LOW means led is *on*
bool hasTriggered = false;     // Flag to prevent multiple calls in 1ms

// Neopixel
#define PIN 7        // On Trinket or Gemma, suggest changing this to 1
#define NUMPIXELS 1  // How many NeoPixels are attached to the Arduino?
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
float ledstate = 0;
float brightness = 1.0;



#ifndef BUILTIN_LED
#ifdef LED_BUILTIN
#define BUILTIN_LED LED_BUILTIN
#else
#define BUILTIN_LED 13
#endif
#endif

void setup() {

  Serial.begin(115200);

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

  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear();  // Set all pixel colors to 'off'
}


void led(OSCMessage &msg) {
  ledState = msg.getFloat(0);
  updateLED();
  Serial.print("/led: ");
  Serial.println(ledState);
}

void ledbrightness(OSCMessage &msg) {
  brightness = msg.getFloat(0);
  updateLED();
  Serial.print("/ledbrightness: ");
  Serial.println(brightness);
}

void updateLED() {
  pixels.clear();  // Set all pixel colors to 'off'
  float color_f = ledState * brightness * 255;
  int color = (int)color_f;
  pixels.setPixelColor(0, pixels.Color(color, color, color));
  pixels.show();
}

void sendMessage() {
  OSCMessage msg("/test");
  msg.add("hello, osc.");
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
}

void loop() {
  OSCMessage msg;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      msg.fill(Udp.read());
    }
    if (!msg.hasError()) {
      msg.dispatch("/led", led);
      msg.dispatch("/ledbrightness", ledbrightness);
    } else {
      error = msg.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }

  // send message every 2 seconds, must be non blocking
 if (millis() % 2000 == 0) {
    if (!hasTriggered) {
      sendMessage();
      hasTriggered = true; // Lock the trigger for this millisecond
    }
  } else {
    // Reset the lock once we move past the 0ms mark
    hasTriggered = false;
  }
}
