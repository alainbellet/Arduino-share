/*---------------------------------------------------------------------------------------------
  
  V1 AB/ECAL 2024
  Communication via OSC 
  Drive Serial Servo from Waveshare - Model SC15 Trough OSC 
  Board Servo driver, ESP32 based
  https://www.waveshare.com/sc15-servo.htm
  https://www.waveshare.com/wiki/SC15_Servo
  https://www.waveshare.com/servo-driver-with-esp32.htm
  https://www.waveshare.com/wiki/Servo_Driver_with_ESP32
  /!\ use max 8.4V power input
  use lib SCServo https://github.com/waveshare/Servo-Driver-with-ESP32
  use lib OSC https://github.com/CNMAT/OSC

  --------------------------------------------------------------------------------------------- */
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <Preferences.h>

#include <SCServo.h>
// Local header files
#include <leds.h>
#include <wifi_udp.h>
#include <servo_ctrl.h>

OSCErrorCode error;
Preferences preferences;  // to save persistent data (board name)


// Timing
unsigned long lastSentMillis = 0;
int sendDelayInMillis = 1000;  // delay for automatically sending values to UR (not event based)

// Servos
int32_t servo_pos = 0;
int32_t servo_position_last = -1;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("");

  // SERVO
  servoInit();
  setMode(1, 3);
  // LEDS
  InitLED();
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
  if (sc.ReadMode(servo_id) == 0) {  // servo mode
    if (servo_position != servo_position_last) {
      sc.WritePos(servo_id, servo_position, 0, 1500);  // use 0xfe as ID for broadcast
      servo_position_last = servo_position;
    }
  }
  if (sc.ReadMode(servo_id) == 3) {
    if (servo_position != servo_position_last) {
      //sc.WritePos(servo_id, servo_position, 0, 1500);  // use 0xfe as ID for broadcast
      sc.WritePos(servo_id, 0, rangeCtrl(servo_position, 0, 999) + 1024, 0);
      servo_position_last = servo_position;
    }
  }
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

void restartESP() {
  Serial.print("Restarting now");
  ESP.restart();
}
