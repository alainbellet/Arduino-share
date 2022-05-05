/*
  Minimal Esp32 Websockets Server

  This sketch:
        1. Connects to a WiFi network
        2. Starts a websocket server on port 80
        3. Waits for connections
        4. Once a client connects, it wait for a message from the client
        5. Sends an "echo" message to the client
        6. closes the connection and goes back to step 3

  Hardware:
        For this sketch you only need an ESP32 board.

  Created 15/02/2019
  By Gil Maimon
  https://github.com/gilmaimon/ArduinoWebsockets
*/

#include <ArduinoWebsockets.h>
#include <WiFi.h>

const char* ssid = "sstudios"; //Enter SSID
const char* password = "quickbrownhunz"; //Enter Password

using namespace websockets;

WebsocketsServer server;
WebsocketsClient client;

int button_pin = 21;
int button_state = -1;
int last_button_state = -1;

void setup() {
  pinMode(button_pin, INPUT_PULLUP);
  Serial.begin(115200);
  // Connect to wifi
  WiFi.begin(ssid, password);

  // Wait some time to connect to wifi
  for (int i = 0; i < 15 && WiFi.status() != WL_CONNECTED; i++) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());   //You can get IP address assigned to ESP

  server.listen(80);
  client = server.accept();
  Serial.print("Is server live? ");
  Serial.println(server.available());
}

void loop() {
  // read button
  String msg_to_send = "";
  button_state = digitalRead(button_pin);
   Serial.println("button: "+button_state);
  if (button_state != last_button_state) {
    Serial.println(button_state);
    //msg_to_send = "Button: " + button_state;
    //client.send(msg_to_send);
    last_button_state = button_state;
  }



  if (client.available()) {

    WebsocketsMessage msg = client.readBlocking();

    // log
    Serial.print("Got Message: ");
    Serial.println(msg.data());

    // return echo
    client.send("Echo: " + msg.data());
    client.send(msg_to_send);
    

    // close the connection
    // client.close();
  }

  // delay(1000);
}
