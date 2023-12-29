
/**
 * Created by K. Suwatchai (Mobizt)
 *
 * Email: k_suwatchai@hotmail.com
 *
 * Github: https://github.com/mobizt/Firebase-ESP-Client
 *
 * Copyright (c) 2023 mobizt
 *
 */

/** This example shows the basic RTDB usage with external Client.


 * Modified A. BELLET to be used on Arduino RP2040 with a simple webpage to control it
 https://ecal-mid.ch/tangible_interface/led/

 Requiert les libriaries
 - Firebase arduino client by Mobizt
 - WiFiNINA by Arduino
 */

#include <SPI.h>
#include <WiFiNINA.h>

#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "ECALEVENT"
#define WIFI_PASSWORD "garamond"

// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyCu-wt8MHNWE6X4MlJIkWmZnUXKWZOOuho"

/* 3. Define the RTDB URL */
#define DATABASE_URL "nano-connect-test-default-rtdb.europe-west1.firebasedatabase.app" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "alain.bellet@ecal.ch"
#define USER_PASSWORD "ecal2023"

// LED RELATED VARS
int red = 0;
int green = 0;
int blue = 0;
int led_builtin = 0;
char boardName[] = "/default/"; // sans accent et en consevant les slashes p.ex "/elodie/"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

WiFiSSLClient ssl_client;

void networkConnection()
{
  // Reset the network connection
  WiFi.disconnect();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  unsigned long ms = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
    if (millis() - ms >= 15000)
    {
      Serial.println(" failed!");
      return;
    }
  }
  Serial.println();
  Serial_Printf("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

// Define the callback function to handle server status acknowledgement
void networkStatusRequestCallback()
{
  // Set the network status
  fbdo.setNetworkStatus(WiFi.status() == WL_CONNECTED);
}

void setup()
{

  Serial.begin(115200);

#if defined(ARDUINO_ARCH_SAMD)
  while (!Serial)
    ;
#endif

  // LED RGB PINS
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  networkConnection();

  Serial_Printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  // The WiFi credentials are required for WiFiNINA and WiFi101 libraries
  // due to it does not have reconnect feature.
  config.wifi.clearAP();
  config.wifi.addAP(WIFI_SSID, WIFI_PASSWORD);

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  /* fbdo.setExternalClient and fbdo.setExternalClientCallbacks must be called before Firebase.begin */

  /* Assign the pointer to global defined external SSL Client object */
  fbdo.setExternalClient(&ssl_client);

  /* Assign the required callback functions */
  fbdo.setExternalClientCallbacks(networkConnection, networkStatusRequestCallback);

  // Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  Firebase.begin(&config, &auth);
}

char *setPath(char value[])
{

  static char path[] = "";
  strcpy(path, boardName);
  strcat(path, value);
  return path;
}
void loop()
{

  // Firebase.ready() should be called repeatedly to handle authentication tasks.

  if (Firebase.ready() && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    red = Firebase.RTDB.getInt(&fbdo, F(setPath("red"))) ? fbdo.intData() : -1;
    green = Firebase.RTDB.getInt(&fbdo, F(setPath("green"))) ? fbdo.intData() : -1;
    blue = Firebase.RTDB.getInt(&fbdo, F(setPath("blue"))) ? fbdo.intData() : -1;
    led_builtin = Firebase.RTDB.getInt(&fbdo, F(setPath("builtinled"))) ? fbdo.intData() : -1;

    // Serial.print(red);
    // Serial.print(green);
    // Serial.println(blue);
    analogWrite(LEDR, abs(255 - red));
    analogWrite(LEDG, abs(255 - green));
    analogWrite(LEDB, abs(255 - blue));
    digitalWrite(LED_BUILTIN, led_builtin);
  }
}
