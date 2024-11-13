/*---------------------------------------------------------------------------------------------

  Controller for UR Engine basic example
  Communication via OSC
  use lib OSC https://github.com/CNMAT/OSC
  V1 AB/ECAL 2022

  --------------------------------------------------------------------------------------------- */

#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <Preferences.h>

#include <ESP32Servo.h>

#define WIFI_SSID "studio"
#define WIFI_PASS "g64HgXG3ngE7PxXaYo"

char ssid[] = WIFI_SSID;  // edit WIFI_SSID + WIFI_PASS constants in the your_secret.h tab (if not present create it)
char pass[] = WIFI_PASS;

WiFiUDP Udp;                          // A UDP instance to let us send and receive packets over UDP
IPAddress outIp(192, 168, 1, 14);     // remote IP of your computer
int outPort = 8888;                   // remote port to send OSC
const unsigned int localPort = 9999;  // local port to listen for OSC packets
bool authorisedIP = false;
IPAddress lastOutIp(10, 189, 8, 81);
char ipAsChar[15];

OSCErrorCode error;
Preferences preferences;  // to save persistent data (board name)

String boardName = "Board_OSC";  // no space in the name

/* ------- Define pins ann vars for button + encoder */
// Button
const int buttonPin = 9;
unsigned int buttonState = 0;
unsigned int buttonLastState = -1;

// Timing
unsigned long lastSentMillis = 0;
int sendDelayInMillis = 1000;  // delay for automatically sending values to UR (not event based)

// Sensor values
int32_t servo_pos = 0;

/* Server for IP table update */
HTTPClient httpclient;

// SERVO
Servo myservo;  // create servo object to control a servo
// 16 servo objects can be created on the ESP32

int pos = 0;  // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33
// Possible PWM GPIO pins on the ESP32-S2: 0(used by on-board button),1-17,18(used by on-board LED),19-21,26,33-42
// Possible PWM GPIO pins on the ESP32-S3: 0(used by on-board button),1-21,35-45,47,48(used by on-board LED)
// Possible PWM GPIO pins on the ESP32-C3: 0(used by on-board button),1-7,8(used by on-board LED),9-10,18-21
#if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
int servoPin = 17;
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
int servoPin = 7;
#else
int servoPin = 13;
#endif

int32_t servo_position_last = -1;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("");

  // SERVO
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);           // standard 50 hz servo
  myservo.attach(servoPin, 500, 2500);  // attaches the servo on pin 18 to the servo object
                                        // using default min/max of 1000us and 2000us
                                        // different servos may require different min/max settings
                                        // for an accurate 0 to 180 sweep

  // BUTTON
  // pinMode(buttonPin, INPUT_PULLUP);

  // Start Wifi and UDP
  startWifiAndUdp();
  // Publish to IP table (online)
  // https://ecal-mid.ch/esp32watcher
  updateIpTable();
}




void loop() {

  

  /* --------- CHECK INCOMMING OSC MSGS */
  OSCMessage msg;
  int size = Udp.parsePacket();
  if (size > 0) {
    while (size--) {
      msg.fill(Udp.read());
    }
    if (!msg.hasError()) {
      msg.dispatch("/arduino/begin", inBegin);
      msg.dispatch("/arduino/servo", inSetServoPosition);
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
void outSendValues() {  // in button, encoder
  // OSCMessage msg("/unreal/state/");
  // msg.add(value_1);
  // msg.add(buttonState);
  // Udp.beginPacket(outIp, outPort);
  // msg.send(Udp);
  // Udp.endPacket();
  // msg.empty();
  // Serial.println("message sent to /unreal/state/");
  // delay(10);
}

/* --------- INCOMMING OSC COMMANDS FUNCTIONS ------------ */

void inBegin(OSCMessage &msg) {  // no value required
  Serial.println("Begin message received");
}

void inSetValue(OSCMessage &msg) {  // int value
  float val = msg.getFloat(0);
  Serial.print("/arduino/value: ");
  Serial.println(val * 100);
}

void inSetServoPosition(OSCMessage &msg) {  // int value
  int32_t servo_id = msg.getInt(0);
  int32_t servo_position = msg.getInt(1);
  if (servo_position != servo_position_last){
    myservo.write(servo_position);
    servo_position_last = servo_position;
  }
  //myservo.write(servo_position);
  
  /*Serial.print("/arduino/servo: ");
  Serial.print(servo_id);
  Serial.print(" ");
  Serial.println(servo_position);*/
}


void inConnect(OSCMessage &msg) {  // string value "ip:port"
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

void inRestartESP(OSCMessage &msg) {  // no value needed
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
  } else {
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
