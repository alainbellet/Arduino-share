/*
  Discord WebHook Example for ESP32
  Based on: https://www.instructables.com/Send-a-Message-on-Discord-Using-Esp32-Arduino-MKR1/
  
  Adapted and commented by Alain Bellet / ECAL

  In order to use this you will need:
  - an esp32 based board
  - a discord webhook
  - an valid SSL certificate to allow https communication 
  - a wifi connection (2.4 GHZ)

  -> Enter your wifi and discord credentials in the "arduino_secrets.h" Tab
  
*/

#include "arduino_secrets.h"
#include "discord.h"

void setup() {
  Serial.begin(115200);
  connectWIFI();
  sendDiscord("esp32 is online");
}

void loop() {
  delay(30 * 1000);
  sendDiscord("i say that every 30 seconds");
}
