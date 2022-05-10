/*
  Library NimBLE
***********************/
#include <NimBLEDevice.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;


#define DEVICE_NAME            "ESP32Printer"
#define SERVICE_UUID           "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID    "6e400001-b5a3-f393-e0a9-e50e24dcca9e"

/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("Device Connected");
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      Serial.println("Device Disconnected");
      deviceConnected = false;
    }
    /***************** New - Security handled here ********************
    ****** Note: these are the same return values as defaults ********/
    uint32_t onPassKeyRequest() {
      Serial.println("Server PassKeyRequest");
      return 123456;
    }

    bool onConfirmPIN(uint32_t pass_key) {
      Serial.print("The passkey YES/NO number: "); Serial.println(pass_key);
      return true;
    }

    void onAuthenticationComplete(ble_gap_conn_desc desc) {
      Serial.println("Starting BLE work!");
    }
    /*******************************************************************/
};

void makeSometingWithMessage(String msg); // constructor

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      // Receive message from web
      std::string rxValue = pCharacteristic->getValue();
      String stringToPrint = "";
      
      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++) {
          Serial.print(rxValue[i]);
          stringToPrint = stringToPrint + rxValue[i];
        }
        Serial.println();
        Serial.println("*********");
      }
      makeSometingWithMessage(stringToPrint);
    }

};

void makeSometingWithMessage(String msg) {
  Serial.print("to print: ");
  Serial.println(msg);
}


void setup() {
  Serial.begin(115200);

  // Create the BLE Device
  BLEDevice::init(DEVICE_NAME);

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      NIMBLE_PROPERTY::READ |
                      NIMBLE_PROPERTY::WRITE  |
                      NIMBLE_PROPERTY::NOTIFY  |
                      NIMBLE_PROPERTY::INDICATE
                    );

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  /** Note, this could be left out as that is the default value */
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter

  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection...");
}

void loop() {

  if (deviceConnected) {
    // SEND Message to WEB
    pCharacteristic->setValue("Hello");
    pCharacteristic->notify();
    delay(1000); // bluetooth stack will go into congestion, if too many packets are sent
  }
  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
}
