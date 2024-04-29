/*---------------------------------------------------------------------------------------------
  Communication via OSC
  use lib OSC https://github.com/CNMAT/OSC (install via Arduino Library Manager)
  V2 AB/ECAL 2024
  UPDATE: Should work with ESP32x and Wifi Nina module (Arduino Nano RP2040 Connect)
  ** LOG
  Use Protokol to log messages https://hexler.net/protokol
  ** TEST
  To test it you can use for example:
  TouchOSC https://hexler.net/touchosc
  GyrOSC fo iOS https://www.bitshapesoftware.com/instruments/gyrosc/
  --------------------------------------------------------------------------------------------- */

#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>  // ESP32
#include <HTTPClient.h>
#include <ESPmDNS.h>
#define WIFI_MODULE_TYPE "esp32"
#else
#include <SPI.h>
#include <WiFiNINA.h>  // Arduino Nano RP2040 Connect
#define WIFI_MODULE_TYPE "nina"
#endif
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include "your_secrets.h"  // Wifi Credentials


// Constants
const char ssid[] = WIFI_SSID;  // edit WIFI_SSID + WIFI_PASS constants in the your_secret.h tab (if not present create it)
const char pass[] = WIFI_PASS;

// UDP
WiFiUDP Udp;                       // A UDP instance to let us send and receive packets over UDP
IPAddress outIp(192, 168, 1, 17);  // remote IP of your computer or other board
int outPort = 8888;                // remote port to send OSC
int inPort = 9999;                 // local port (this board) to listen for OSC packets
// if you have 2 boards speaking to eachother, set one side to 1 and other side 2
const int board_side = 1;

// OSC
OSCErrorCode error;
char boardName[30] = "Board_Please_Rename";  // PLEASE EDIT - no space in the name
/* Server for IP table update */
#if defined(ARDUINO_ARCH_ESP32)
HTTPClient httpclient;
#else
WiFiSSLClient httpclient;
#endif

/* ------- Define pins and vars for button + encoder */
// Button
const int buttonPin = 2;
unsigned int buttonState = 0;
unsigned int buttonLastState = -1;

// Timing
unsigned long lastSentMillis = 0;
int sendDelayInMillis = 5000;  // delay for automatically sending values to remote (not event based)
// Example value
int32_t value_1 = 0;

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("setup");
  if (board_side == 2) {  // switch in and out port for the other side if two boards
    outPort = 9999;
    inPort = 8888;
    Serial.println("board 2");
  }
  // BUTTON
  pinMode(buttonPin, INPUT_PULLUP);
  // LED
  pinMode(LED_BUILTIN, OUTPUT);
  // Start Wifi and UDP
  startWifiAndUdp();
  // Publish to IP table (online)
  // https://ecal-mid.ch/esp32watcher
  updateIpTable();
#if defined(ARDUINO_ARCH_ESP32)
  start_mdns_service();  // start MDNS discovaribilty (zeroconf) [boardName].local
#endif
}

void loop() {
  /* --------- SEND OSC MSGS */
  // BUTTON
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  if (buttonState != buttonLastState) {
    outSendButton();
    buttonLastState = buttonState;
    delay(50);
  }

  if (millis() - lastSentMillis > sendDelayInMillis) {
    lastSentMillis = millis();
    value_1 = value_1 + 1;
    // send values regularely
    outSendValues();
  }

  /* --------- CHECK INCOMMING OSC MSGS */
  OSCMessage msgIN;
  OSCBundle bundleIN;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      uint8_t incomingByte = Udp.read();
      msgIN.fill(incomingByte);
      bundleIN.fill(incomingByte);
    }
    if (!msgIN.hasError()) {
      // simple messages here (for ex. from arduino, chataigne)
      msgIN.dispatch("/arduino/value", inValue);
      msgIN.dispatch("/arduino/button", inButton);
    }

    if (!bundleIN.hasError()) {
      // messages in bundles here (for ex. from GyrOSC, TouchOSC)
      bundleIN.dispatch("/gyrosc/gyro", inGyroscGyro);
      bundleIN.dispatch("/gyrosc/comp", inGyroscComp);
      bundleIN.dispatch("/touchosc/radial", inRadial);
    }
  }
}

/* --------- OUTGOING OSC COMMANDS FUNCTIONS ------------ */
void outSendValues() {
  OSCMessage msg("/arduino/value");
  msg.add(value_1);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
  //Serial.println("message sent to /arduino/value");
}

void outSendButton() {
  OSCMessage msg("/arduino/button");
  msg.add(buttonState);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
  //Serial.println("message sent to /arduino/button");
}
/* --------- INCOMMING OSC COMMANDS FUNCTIONS ------------ */

void inBegin(OSCMessage &msg) {  // no value required
  Serial.println("Begin message received");
}

void inButton(OSCMessage &msg) {  // int value
  int btn_state = msg.getInt(0);
  Serial.print("/arduino/button: ");
  Serial.println(btn_state);
  // set LED
  digitalWrite(LED_BUILTIN, btn_state);
}

void inRadial(OSCMessage &msg) {  // int value
  float radial_knob = msg.getFloat(0);
  Serial.print("/touchosc/radial ");
  Serial.println(radial_knob);
}

void inValue(OSCMessage &msg) {  // int value
  int val = msg.getInt(0);
  Serial.print("/arduino/value: ");
  Serial.println(val);
}

// from ios app GyrOSC https://www.bitshapesoftware.com/instruments/gyrosc/
void inGyroscGyro(OSCMessage &msg) {
  // Gyro data
  float pitch = msg.getFloat(0);
  float roll = msg.getFloat(1);
  float yaw = msg.getFloat(2);
  Serial.print("/gyrosc/gyro ");
  Serial.print(pitch);
  Serial.print(" ");
  Serial.print(roll);
  Serial.print(" ");
  Serial.println(yaw);
}

void inGyroscComp(OSCMessage &msg) {
  // Compass data
  float heading = msg.getFloat(0);
  Serial.print("/gyrosc/comp ");
  Serial.println(heading);
}

/* --------- OTHER FUNCTIONS ------------ */

void startWifiAndUdp() {
  Serial.println();
  Serial.println();
#if !defined(ARDUINO_ARCH_ESP32)
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  } else {
    Serial.println("Communication with WiFi module succesfull!");
  }
#endif
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  int tryCount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // Start UDP
  Serial.println("Starting UDP");
  if (!Udp.begin(inPort)) {
    Serial.println("Error starting UDP");
    return;
  } else {
    Serial.println("UDP started succesfully");
  }
  Serial.print("Remote IP: ");
  Serial.println(outIp);
  Serial.print("Remote Port: ");
  Serial.println(outPort);
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Local Port: ");
  Serial.println(inPort);
}

void printOSCError(OSCErrorCode error, int type) {
  if (type == 1) {
    Serial.print("OSC message error: ");
  }
  if (type == 2) {
    Serial.print("OSC bundle error: ");
  }
  Serial.println(error);
}

#if defined(ARDUINO_ARCH_ESP32)
void updateIpTable() {
  httpclient.begin("https://ecal-mid.ch/esp32watcher/update.php?name=" + String(boardName) + "_" + WIFI_MODULE_TYPE + "&ip=" + WiFi.localIP().toString() + "&wifi=" + WIFI_SSID);
  int httpResponseCode = httpclient.GET();
  httpclient.end();
}

void start_mdns_service() {
  //initialize mDNS service
  esp_err_t err = mdns_init();
  if (err) {
    Serial.println("MDNS Init failed");
    return;
  } else {
    Serial.println("MDNS started: " + String(boardName)+".local");
  }
  //set hostname
  mdns_hostname_set(boardName);
}
#else
void updateIpTable() {
  if (httpclient.connect("ecal-mid.ch", 443)) {
    httpclient.print(String("GET /esp32watcher/update.php?name=") + boardName + "_" + WIFI_MODULE_TYPE + "&ip=" + WiFi.localIP().toString() + "&wifi=" + WiFi.SSID() + " HTTP/1.1\r\n" + "Host: ecal-mid.ch\r\n" + "Connection: close\r\n\r\n");
  }
}
#endif
