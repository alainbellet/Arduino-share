#include "arduino_secrets.h"
/*
  Discord WebHook Example for ESP32
*/
#include "discord.h"

void setup() {
  Serial.begin(9600);
  connectWIFI();
  sendDiscord("je dis ça, je dis rien!");
}

void loop() {
  delay(20 * 1000);
  sendDiscord("je dis la meme chose toutes les 20 secondes");
}
