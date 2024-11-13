#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>

#define WIFI_SSID "ECALEVENT"
#define WIFI_PASS "****"

char ssid[] = WIFI_SSID;  // edit WIFI_SSID + WIFI_PASS constants in the your_secret.h tab (if not present create it)
char pass[] = WIFI_PASS;

WiFiUDP Udp;                          // A UDP instance to let us send and receive packets over UDP
IPAddress outIp(192, 168, 1, 14);     // remote IP of your computer
int outPort = 8888;                   // remote port to send OSC
const unsigned int localPort = 9999;  // local port to listen for OSC packets
bool authorisedIP = false;
IPAddress lastOutIp(10, 189, 8, 81);
char ipAsChar[15];

/* Server for IP table update */
HTTPClient httpclient;
String boardName = "SERVO_CONTROL_OSC";  // no space in the name

void startWifiAndUdp() {
  // Connect to WiFi network
  LEDSetSingle(0, matrix.Color(255, 0, 0));// red
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
  LEDSetSingle(0, matrix.Color(0, 255, 0));// green
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
