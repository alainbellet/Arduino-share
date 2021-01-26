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
  //sendDiscord("Hello de l'esp32! (prochain message dans 20 secondes)");
  delay(20 * 1000);
}
