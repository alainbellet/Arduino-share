/*
 * ESP32 BLE Keyboard - Button to Keystroke
 *
 * This sketch turns an ESP32 into a Bluetooth Low Energy (BLE) HID keyboard.
 * Physical buttons connected to GPIO pins send key press/release events to
 * a connected device (computer, phone, tablet).
 *
 * Features:
 * - Native ESP32 BLE implementation (no external library needed)
 * - Supports lowercase letters a-z
 * - Debounced button input
 * - Up to 6 simultaneous key presses (standard USB HID keyboard spec)
 */

// ESP32 BLE libraries (built into ESP32 Arduino core)
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLEHIDDevice.h>
#include <BLECharacteristic.h>

// ========== CONFIGURATION ==========
// Adjust these values for your setup

static const uint8_t BUTTON_COUNT = 4;                                // Number of buttons
static const uint8_t buttonPins[BUTTON_COUNT] = {0, 1, 2, 3};         // GPIO pins (INPUT_PULLUP: button connects pin to GND)
static const uint8_t buttonKeys[BUTTON_COUNT] = {'a', 'b', 'c', 'd'}; // Keys to send (lowercase a-z only)

// ========== DEBOUNCE STATE ==========
// Track button state changes to filter electrical noise

static bool stablePressed[BUTTON_COUNT];    // Current stable (debounced) button state
static bool lastRawPressed[BUTTON_COUNT];   // Last raw reading from digitalRead
static uint32_t lastChangeMs[BUTTON_COUNT]; // Timestamp of last state change
static const uint32_t debounceMs = 25;      // Debounce time in milliseconds (increase if buttons are noisy)

// ========== BLE OBJECTS ==========

BLEHIDDevice *hid;                // BLE HID device interface
BLECharacteristic *inputKeyboard; // Characteristic for sending keyboard reports
bool connected = false;           // Connection status flag

// ========== HID REPORT DESCRIPTOR ==========
// Defines the keyboard report structure according to USB HID specification
// This tells the host device what kind of data to expect from this keyboard

static const uint8_t _hidReportDescriptor[] = {
    0x05, 0x01, // Usage Page (Generic Desktop)
    0x09, 0x06, // Usage (Keyboard)
    0xA1, 0x01, // Collection (Application)
    0x85, 0x01, //   Report ID (1)
    0x05, 0x07, //   Usage Page (Key Codes)
    0x19, 0xE0, //   Usage Minimum (224)  - Modifier keys start
    0x29, 0xE7, //   Usage Maximum (231)  - Modifier keys end (Ctrl, Shift, Alt, GUI)
    0x15, 0x00, //   Logical Minimum (0)
    0x25, 0x01, //   Logical Maximum (1)
    0x75, 0x01, //   Report Size (1 bit per modifier)
    0x95, 0x08, //   Report Count (8 modifiers)
    0x81, 0x02, //   Input (Data, Variable, Absolute) - 1 byte for modifiers
    0x95, 0x01, //   Report Count (1)
    0x75, 0x08, //   Report Size (8)
    0x81, 0x01, //   Input (Constant) - 1 reserved byte
    0x95, 0x06, //   Report Count (6) - Up to 6 simultaneous keys
    0x75, 0x08, //   Report Size (8 bits per key)
    0x15, 0x00, //   Logical Minimum (0)
    0x25, 0x65, //   Logical Maximum (101)
    0x05, 0x07, //   Usage Page (Key Codes)
    0x19, 0x00, //   Usage Minimum (0)
    0x29, 0x65, //   Usage Maximum (101)
    0x81, 0x00, //   Input (Data, Array) - 6 bytes for key codes
    0xC0        // End Collection
};

// ========== KEYBOARD REPORT STRUCTURE ==========
// Standard USB HID keyboard report format (8 bytes total)

typedef struct
{
  uint8_t modifiers; // Byte 0: Modifier keys (Ctrl, Shift, Alt, GUI) - not used in this sketch
  uint8_t reserved;  // Byte 1: Reserved (always 0)
  uint8_t keys[6];   // Bytes 2-7: Up to 6 simultaneous key codes (6-key rollover)
} KeyReport;

// Global keyboard report - sent to host on every key press/release
KeyReport keyReport = {0, 0, {0, 0, 0, 0, 0, 0}};

// ========== BLE CONNECTION CALLBACKS ==========
// Handle connection and disconnection events

class ServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    connected = true;
    Serial.println("BLE Keyboard Connected");
  }

  void onDisconnect(BLEServer *pServer)
  {
    connected = false;
    Serial.println("BLE Keyboard Disconnected");
    BLEDevice::startAdvertising(); // Restart advertising to allow reconnection
  }
};

// ========== KEY MAPPING ==========
// Convert ASCII character to HID usage code
// HID usage codes: 'a'=0x04, 'b'=0x05, ..., 'z'=0x1D

uint8_t keyToHidCode(uint8_t key)
{
  // Only lowercase a-z supported
  if (key >= 'a' && key <= 'z')
    return 0x04 + (key - 'a'); // Map a-z to HID codes 0x04-0x1D
  return 0;                    // Invalid key
}

// ========== KEY PRESS HANDLER ==========
// Add a key to the report and send it to the connected device

void pressKey(uint8_t key)
{
  uint8_t hidCode = keyToHidCode(key);
  if (hidCode == 0)
    return; // Invalid key, ignore

  // Find first empty slot in the 6-key array and add this key
  for (int i = 0; i < 6; i++)
  {
    if (keyReport.keys[i] == 0)
    {
      keyReport.keys[i] = hidCode;
      break; // Stop after adding to first available slot
    }
  }

  // Send the updated report to the connected device
  if (connected)
  {
    inputKeyboard->setValue((uint8_t *)&keyReport, sizeof(keyReport));
    inputKeyboard->notify(); // Trigger BLE notification to host
    Serial.print("SENT: Press '");
    Serial.print((char)key);
    Serial.println("'");
  }
}

// ========== KEY RELEASE HANDLER ==========
// Remove a key from the report and send the update to the connected device

void releaseKey(uint8_t key)
{
  uint8_t hidCode = keyToHidCode(key);
  if (hidCode == 0)
    return; // Invalid key, ignore

  // Find this key in the array and clear it
  for (int i = 0; i < 6; i++)
  {
    if (keyReport.keys[i] == hidCode)
    {
      keyReport.keys[i] = 0; // Clear the slot
      break;                 // Stop after finding and clearing the key
    }
  }

  // Send the updated report to the connected device
  if (connected)
  {
    inputKeyboard->setValue((uint8_t *)&keyReport, sizeof(keyReport));
    inputKeyboard->notify(); // Trigger BLE notification to host
    Serial.print("SENT: Release '");
    Serial.print((char)key);
    Serial.println("'");
  }
}

// ========== SETUP ==========
// Initialize buttons and BLE keyboard

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting BLE Keyboard");

  // Initialize button pins and debounce state
  for (uint8_t i = 0; i < BUTTON_COUNT; i++)
  {
    pinMode(buttonPins[i], INPUT_PULLUP);                // Enable internal pull-up resistor
    bool pressed = (digitalRead(buttonPins[i]) == HIGH); // HIGH = button pressed (inverted logic)
    stablePressed[i] = pressed;
    lastRawPressed[i] = pressed;
    lastChangeMs[i] = millis();
  }

  // Initialize BLE device
  BLEDevice::init("ESP32 Keyboard"); // Device name visible during pairing
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks()); // Register connection callbacks

  // Create HID device
  hid = new BLEHIDDevice(pServer);
  inputKeyboard = hid->inputReport(1); // Report ID 1 (matches descriptor)

  // Set device information
  hid->manufacturer()->setValue("Espressif");
  hid->pnp(0x02, 0x05ac, 0x820a, 0x0210); // PnP ID (vendor, product, version)
  hid->hidInfo(0x00, 0x01);               // HID version

  // Configure security (pairing with PIN/passkey)
  BLESecurity *pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND);

  // Register HID report descriptor and start services
  hid->reportMap((uint8_t *)_hidReportDescriptor, sizeof(_hidReportDescriptor));
  hid->startServices();

  // Start advertising (make device discoverable)
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->setAppearance(0x03C1); // HID Keyboard appearance
  pAdvertising->addServiceUUID(hid->hidService()->getUUID());
  pAdvertising->start();

  hid->setBatteryLevel(100); // Report 100% battery
  Serial.println("BLE Keyboard ready");
  Serial.println("Waiting for connection...");
}

// ========== MAIN LOOP ==========
// Continuously scan buttons and send key events

void loop()
{
  const uint32_t now = millis();

  // Check each button
  for (uint8_t i = 0; i < BUTTON_COUNT; i++)
  {
    const bool rawPressed = (digitalRead(buttonPins[i]) == HIGH); // Read current state (inverted logic)

    // Detect state change (start debounce timer)
    if (rawPressed != lastRawPressed[i])
    {
      lastRawPressed[i] = rawPressed;
      lastChangeMs[i] = now; // Reset debounce timer
    }

    // If state has been stable for debounceMs, accept the change
    if ((now - lastChangeMs[i]) >= debounceMs && rawPressed != stablePressed[i])
    {
      stablePressed[i] = rawPressed; // Update stable state

      // Button pressed (LOW = pressed with INPUT_PULLUP)
      if (stablePressed[i])
      {
        Serial.print("Button ");
        Serial.print(i);
        Serial.println(" pressed");
        pressKey(buttonKeys[i]); // Send key press event
      }
      // Button released
      else
      {
        Serial.print("Button ");
        Serial.print(i);
        Serial.println(" released");
        releaseKey(buttonKeys[i]); // Send key release event
      }
    }
  }

  delay(1); // Small delay to prevent excessive CPU usage
}
