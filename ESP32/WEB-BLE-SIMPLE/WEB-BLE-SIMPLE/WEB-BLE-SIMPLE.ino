#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define BUTTON_PIN_1 0
#define BUTTON_PIN_2 1
#define BUTTON_PIN_3 2
#define SERVICE_UUID "12345678-1234-1234-1234-123456789012"
#define CHARACTERISTIC_UUID_1 "87654321-4321-4321-4321-210987654321"
#define CHARACTERISTIC_UUID_2 "87654321-4321-4321-4321-210987654322"
#define CHARACTERISTIC_UUID_3 "87654321-4321-4321-4321-210987654323"

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic1 = NULL;
BLECharacteristic *pCharacteristic2 = NULL;
BLECharacteristic *pCharacteristic3 = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t buttonState1 = 0;
uint8_t lastButtonState1 = 0;
uint8_t buttonState2 = 0;
uint8_t lastButtonState2 = 0;
uint8_t buttonState3 = 0;
uint8_t lastButtonState3 = 0;

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Device connected");
  };

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Device disconnected");
  }
};

void setup()
{
  Serial.begin(115200);
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  pinMode(BUTTON_PIN_3, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println("Starting BLE work!");

  BLEDevice::init("ESP32-C3-Button");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic1 = pService->createCharacteristic(
      CHARACTERISTIC_UUID_1,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_INDICATE);
  pCharacteristic1->addDescriptor(new BLE2902());

  pCharacteristic2 = pService->createCharacteristic(
      CHARACTERISTIC_UUID_2,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_INDICATE);
  pCharacteristic2->addDescriptor(new BLE2902());

  pCharacteristic3 = pService->createCharacteristic(
      CHARACTERISTIC_UUID_3,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_INDICATE);
  pCharacteristic3->addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();
  Serial.println("Waiting for a client connection to notify...");
}

void loop()
{
  buttonState1 = digitalRead(BUTTON_PIN_1);
  buttonState2 = digitalRead(BUTTON_PIN_2);
  buttonState3 = digitalRead(BUTTON_PIN_3);

  if (buttonState1 != lastButtonState1)
  {
    lastButtonState1 = buttonState1;
    if (deviceConnected)
    {
      uint8_t value = (buttonState1 == LOW) ? 1 : 0;
      pCharacteristic1->setValue(&value, 1);
      pCharacteristic1->notify();
      Serial.print("Button 1 state changed: ");
      Serial.println(value);
    }
    delay(50);
  }

  if (buttonState2 != lastButtonState2)
  {
    lastButtonState2 = buttonState2;
    if (deviceConnected)
    {
      uint8_t value = (buttonState2 == LOW) ? 1 : 0;
      pCharacteristic2->setValue(&value, 1);
      pCharacteristic2->notify();
      Serial.print("Button 2 state changed: ");
      Serial.println(value);
    }
    delay(50);
  }

  if (buttonState3 != lastButtonState3)
  {
    lastButtonState3 = buttonState3;
    if (deviceConnected)
    {
      uint8_t value = (buttonState3 == LOW) ? 1 : 0;
      pCharacteristic3->setValue(&value, 1);
      pCharacteristic3->notify();
      Serial.print("Button 3 state changed: ");
      Serial.println(value);
    }
    delay(50);
  }

  if (!deviceConnected && oldDeviceConnected)
  {
    delay(500);
    pServer->startAdvertising();
    Serial.println("Start advertising");
    oldDeviceConnected = deviceConnected;
  }

  if (deviceConnected && !oldDeviceConnected)
  {
    oldDeviceConnected = deviceConnected;
  }

  delay(10);
}
