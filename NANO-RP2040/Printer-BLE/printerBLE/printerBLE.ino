/*
Example for Arduino Nana RP2040 connect
A.B. ECAL 2022

*/

#include <ArduinoBLE.h>

BLEService printservice("6e400001-b5a3-f393-e0a9-e50e24dcca9e");

BLEStringCharacteristic stringCharacteristic("6e400001-b5a3-f393-e0a9-e50e24dcca9e", BLEWrite, 200); // 20 max length
BLEIntCharacteristic buttonCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

const int ledPin = LED_BUILTIN; // pin to use for the LED
const int btnPin = 4; // pin to use for the BUTTON
int buttonState;
int lastButtonState = 1;

void setup() {
  Serial.begin(9600);
  delay(500);

  // set Button pin to input mode
  pinMode(btnPin, INPUT_PULLUP);
  // set LED pin to output mode
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("BLEprinter");
  BLE.setDeviceName("BLEprinter");
  BLE.setAdvertisedServiceUuid(printservice.uuid());
  BLE.setAdvertisedService(printservice);

  printservice.addCharacteristic(stringCharacteristic);
  printservice.addCharacteristic(buttonCharacteristic);

  // add service
  BLE.addService(printservice);



  // start advertising
  BLE.advertise();

  Serial.println("BLE Print Peripheral started");
}

void loop() {
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    BLE.poll();
    Serial.print("Connected to central: ");
    // if connected turn led ON
    digitalWrite(ledPin, HIGH);

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // check if button pressed
      buttonState = digitalRead(btnPin);
      if (buttonState != lastButtonState) {
        if (buttonState == 0) { // button pressed (connect to gnd)
          buttonCharacteristic.writeValue(1);
        }else{
          buttonCharacteristic.writeValue(0);
        }
        lastButtonState = buttonState;
        delay(10);
      }

      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (stringCharacteristic.written()) {
        if (stringCharacteristic.value()) {   // any value other than 0
          Serial.print("max String size = ");
          Serial.println(stringCharacteristic.valueSize());
          Serial.print("received String size = ");
          Serial.println(stringCharacteristic.valueLength());
          String valString = stringCharacteristic.value();
          Serial.println(valString);

        }
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    // if disconnected turn led ON
    digitalWrite(ledPin, LOW);
  }
}
