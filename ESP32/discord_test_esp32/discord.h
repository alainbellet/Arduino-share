#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

WiFiMulti WiFiMulti;

void connectWIFI() {
  WiFiMulti.addAP(ssid, pass);
  WiFi.mode(WIFI_STA);
  Serial.print("[WiFi] Connecting to: ");
  Serial.println(ssid);
  // wait for WiFi connection
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println("[WiFi] Connected");
}

void sendDiscord(String content) {
  WiFiClientSecure *client = new WiFiClientSecure;
  if (client) {
    client -> setCACert(discordappCertificate);
    {
      HTTPClient https;
      Serial.println("[HTTP] Connecting to Discord...");
      Serial.println("[HTTP] Message: " + content);
      Serial.println("[HTTP] TTS: " + discord_tts);
      if (https.begin(*client, discord_webhook)) {  // HTTPS
        // start connection and send HTTP header
        https.addHeader("Content-Type", "application/json");
        int httpCode = https.POST("{\"content\":\"" + content + "\",\"tts\":" + discord_tts +"}");

        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.print("[HTTP] Status code: ");
          Serial.println(httpCode);

          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.print("[HTTP] Response: ");
            Serial.println(payload);
          }
        } else {
          Serial.print("[HTTP] Post... failed, error: ");
          Serial.println(https.errorToString(httpCode).c_str());
        }

        https.end();
      } else {
        Serial.printf("[HTTP] Unable to connect\n");
      }

      // End extra scoping block
    }

    delete client;
  } else {
    Serial.println("[HTTP] Unable to create client");
  }
}
