/*---------------------------------------------------------------------------------------------

  Controller for UR Engine basic example
  Communication via OSC

  V1 AB/ECAL 2022

  --------------------------------------------------------------------------------------------- */
#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include "your_secrets.h"
#include <Preferences.h>

char ssid[] = WIFI_SSID;                    // edit WIFI_SSID + WIFI_PASS constants in the your_secret.h tab (if not present create it)
char pass[] = WIFI_PASS;

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
IPAddress outIp(10, 189, 8, 81);            // remote IP of your computer
int outPort = 8888;                         // remote port to send OSC
const unsigned int localPort = 9999;        // local port to listen for OSC packets
bool authorisedIP = false;
IPAddress lastOutIp(10, 189, 8, 81);
char ipAsChar[15];

OSCErrorCode error;
Preferences preferences; // to save persistent data (board name)

String boardName = "Board_Alain"; // no space in the name

/* ------- Define pins ann vars for button + encoder */
// Button
const int buttonPin = 27;
int buttonState = 0;
int buttonLastState = -1;

// Timing
unsigned long lastSentMillis = 0;
int sendDelayInMillis = 500; // delay for automatically sending values to UR (not event based)

// Sensor values
int value_1 = 0;

/* Server for IP table update */
HTTPClient httpclient;


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("");

  // BUTTON
  pinMode(buttonPin, INPUT_PULLUP);

  // Start Wifi and UDP
  startWifiAndUdp();
  // Publish to IP table (online)
  // https://ecal-mid.ch/esp32watcher
  updateIpTable();
}


void loop() {

  /* --------- SEND OSC MSGS */

  // BUTTON
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  if (buttonState != buttonLastState) {
    outSendValues();
    buttonLastState = buttonState;
  }

  if (millis() - lastSentMillis > sendDelayInMillis) {
    lastSentMillis = millis();
    value_1 = value_1 + 1;
    // send values regularely
    outSendValues();
  }

  /* --------- CHECK INCOMMING OSC MSGS */
  OSCMessage msg;
  int size = Udp.parsePacket();
  if (size > 0) {
    while (size--) {
      msg.fill(Udp.read());
    }
    if (!msg.hasError()) {
      msg.dispatch("/arduino/begin", inBegin);
      msg.dispatch("/arduino/value", inSetValue);
      msg.dispatch("/arduino/restart", inRestartESP);
    } else {
      error = msg.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }
}

/* --------- FUNCTIONS ------------ */


/* --------- OUTGOING OSC COMMANDS FUNCTIONS ------------ */
void outSendValues() { // in button, encoder
  OSCMessage msg("/unreal/state/");
  msg.add(value_1);
  msg.add(buttonState);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
  Serial.println("message sent to /unreal/state/");
  delay (10);
}

/* --------- INCOMMING OSC COMMANDS FUNCTIONS ------------ */

void inBegin(OSCMessage &msg) { // no value required
 Serial.println("Begin message received");
}

void inSetValue(OSCMessage &msg) { // int value 
  int val = msg.getInt(0);
  Serial.print("/arduino/value: ");
  Serial.println(val);
}


void inConnect(OSCMessage &msg) { // string value "ip:port"
  /*lastKeepAliveReceived = millis();
  clientConnected = true;
  char newIpAndPort[20];
  int str_length = msg.getString(0, newIpAndPort, 20);
  String ipAndportString = String(newIpAndPort);
  // split IP and Port
  int colonPos = ipAndportString.indexOf(":");
  String ipString = ipAndportString.substring(0, colonPos);
  String PortString = ipAndportString.substring(colonPos + 1, ipAndportString.length());
  outIp.fromString(ipString);
  outPort = PortString.toInt();
  // save iP as Char Array for sending
  WiFi.localIP().toString().toCharArray(ipAsChar, 15);
  Serial.print("New remote IP: ");
  Serial.println(outIp);
  Serial.print("New remote Port: ");
  Serial.println(outPort);*/
}

void inRestartESP(OSCMessage &msg) { // no value needed
  restartESP();
}


/* --------- OTHER FUNCTIONS ------------ */

void startWifiAndUdp() {
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
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // Start UDP
  Serial.println("Starting UDP");
  if (!Udp.begin(localPort)) {
    Serial.println("Error starting UDP");
    return;
  }
  Serial.print("Local port: ");
  Serial.println(localPort);
  Serial.print("Remote IP: ");
  Serial.println(outIp);
}

void updateIpTable() {
  httpclient.begin("https://ecal-mid.ch/esp32watcher/update.php?name=" + boardName + "&ip=" + WiFi.localIP().toString() + "&wifi=" + WIFI_SSID);
  int httpResponseCode = httpclient.GET();
  if (httpResponseCode > 0) {
    String payload = httpclient.getString();
    Serial.println(payload);
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  httpclient.end();
}

void restartESP() {
  Serial.print("Restarting now");
  ESP.restart();
}
