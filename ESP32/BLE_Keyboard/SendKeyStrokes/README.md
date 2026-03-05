# ESP32 BLE Keyboard - Button to Keystroke

Transform your ESP32 into a wireless Bluetooth keyboard that sends keystrokes when physical buttons are pressed. Perfect for custom macro pads, accessibility devices, or game controllers.

## Features

- ✅ **Native BLE implementation** - No external libraries required (uses ESP32 Arduino core BLE)
- ✅ **Lowercase letters a-z** - Simple character support
- ✅ **Debounced input** - Filters electrical noise from buttons
- ✅ **6-key rollover** - Press up to 6 keys simultaneously (standard USB HID spec)
- ✅ **Auto-reconnect** - Automatically advertises after disconnection
- ✅ **Serial debug output** - Monitor button presses and BLE events

## Hardware Requirements

### Components
- **Lolin ESP32-C3 Pico board** (or any ESP32-C3 variant with BLE support)
- **Push buttons** (normally open, momentary)
- **Optional:** Pull-down resistors (10kΩ) if not using INPUT_PULLUP mode

> **Note:** This code is specifically tested on the **Lolin ESP32-C3 Pico** board. The ESP32-C3 is a single-core RISC-V chip with built-in BLE 5.0 support.

### Wiring

The code uses `INPUT_PULLUP` mode, so wiring is simple:

```
Button 0: GPIO 0  ──┬── Button ── GND
Button 1: GPIO 1  ──┤
Button 2: GPIO 2  ──┤
Button 3: GPIO 3  ──┘
```

**Each button connects:**
- One side → GPIO pin
- Other side → GND (ground)

**No external resistors needed** - the ESP32's internal pull-up resistors are enabled.

### ⚠️ GPIO Pin Recommendations for ESP32-C3 Pico

The **Lolin ESP32-C3 Pico** has limited GPIO pins. Safe pins for buttons:

**Recommended pins:** GPIO 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10

**Avoid these pins:**
- **GPIO 18, 19** - USB D- and D+ (used for programming/serial)
- **GPIO 12, 13** - SPI Flash (SPIHD, SPIWP)
- **GPIO 14, 15, 16, 17** - SPI Flash (SPICS0, SPICLK, SPID, SPIQ)

**Current configuration uses:** GPIO 0, 1, 2, 3 (safe for ESP32-C3)

To change pins, edit line 26 in the sketch:
```cpp
static const uint8_t buttonPins[BUTTON_COUNT] = {4, 5, 6, 7};  // Alternative pins
```

## Software Requirements

### Arduino IDE Setup

1. **Install ESP32 Board Support**
   - Open Arduino IDE
   - Go to **File → Preferences**
   - Add to "Additional Board Manager URLs":
     ```
     https://espressif.github.io/arduino-esp32/package_esp32_index.json
     ```
   - Go to **Tools → Board → Boards Manager**
   - Search for "ESP32" and install **esp32 by Espressif Systems**

2. **Select Your Board**
   - Go to **Tools → Board → ESP32 Arduino**
   - Select **"ESP32C3 Dev Module"** (for Lolin ESP32-C3 Pico)
   - If you don't see ESP32-C3 boards, ensure you have ESP32 Arduino core **2.0.0 or newer**

3. **Configure Upload Settings**
   - **Port:** Select your ESP32's COM/serial port
   - **Upload Speed:** 115200 (or higher if supported)

### No External Libraries Needed

This sketch uses only the built-in ESP32 BLE libraries:
- `BLEDevice.h`
- `BLEServer.h`
- `BLEUtils.h`
- `BLEHIDDevice.h`
- `BLECharacteristic.h`

These are included with the ESP32 Arduino core (version 3.3.7 tested).

## Installation

1. **Download the sketch**
   - Copy `SendKeyStrokes.ino` to your Arduino sketchbook

2. **Open in Arduino IDE**
   - File → Open → `SendKeyStrokes.ino`

3. **Configure your setup** (optional)
   - Edit lines 25-27 to match your hardware:
   ```cpp
   static const uint8_t BUTTON_COUNT = 4;
   static const uint8_t buttonPins[BUTTON_COUNT] = {0, 1, 2, 3};
   static const uint8_t buttonKeys[BUTTON_COUNT] = {'a', 'b', 'c', 'd'};
   ```

4. **Upload to ESP32**
   - Click **Upload** button (→)
   - Wait for "Done uploading"

## Usage

### 1. Power On & Pair

1. Upload the sketch and open **Serial Monitor** (115200 baud)
2. You should see:
   ```
   Starting BLE Keyboard
   BLE Keyboard ready
   Waiting for connection...
   ```

3. On your device (computer/phone/tablet):
   - Open Bluetooth settings
   - Look for **"ESP32 Keyboard"**
   - Click to pair
   - Enter PIN if prompted (security pairing)

4. When connected, Serial Monitor shows:
   ```
   BLE Keyboard Connected
   ```

### 2. Use the Buttons

Press any button and the corresponding key will be sent:

**Serial Monitor output:**
```
Button 0 pressed
SENT: Press 'a'
Button 0 released
SENT: Release 'a'
```

**On your device:** The letter appears wherever your cursor is (text editor, browser, etc.)

### 3. Reconnection

If disconnected:
```
BLE Keyboard Disconnected
```

The ESP32 automatically starts advertising again. Just reconnect from your device's Bluetooth settings.

## Configuration

### Change Number of Buttons

Edit line 25:
```cpp
static const uint8_t BUTTON_COUNT = 6;  // Change to your number of buttons
```

Then update the arrays:
```cpp
static const uint8_t buttonPins[BUTTON_COUNT] = {25, 26, 27, 32, 33, 14};
static const uint8_t buttonKeys[BUTTON_COUNT] = {'a', 'b', 'c', 'd', 'e', 'f'};
```

### Change Keys

Edit line 27 to assign different letters:
```cpp
static const uint8_t buttonKeys[BUTTON_COUNT] = {'q', 'w', 'e', 'r'};  // QWER layout
```

**Supported keys:** Lowercase letters `a-z` only (for now)

### Adjust Debounce Time

If buttons are triggering multiple times per press, increase the debounce:

Edit line 35:
```cpp
static const uint32_t debounceMs = 50;  // Increase from 25 to 50ms
```

### Change Device Name

Edit line 197:
```cpp
BLEDevice::init("My Custom Keyboard");  // Change the name
```

This name appears when pairing.

## Troubleshooting

### ESP32 won't upload / keeps resetting
- **Cause:** GPIO 0 held LOW at boot enters flash mode
- **Solution:** Use different pins (25, 26, 27, 32, 33)

### Serial Monitor shows garbage characters
- **Cause:** GPIO 1 is UART TX
- **Solution:** Use a different pin for Button 1

### Keys "stick" (don't release)
- **Cause:** Release event not sent
- **Solution:** Check Serial Monitor for "SENT: Release" messages. If missing, check button wiring.

### Multiple key presses per button press
- **Cause:** Button bounce (electrical noise)
- **Solution:** Increase `debounceMs` from 25 to 50 or higher

### Can't find "ESP32 Keyboard" in Bluetooth
- **Cause:** BLE not initialized or wrong board selected
- **Solution:** 
  - Check Serial Monitor for "BLE Keyboard ready"
  - Verify ESP32 board is selected in Tools → Board
  - Try resetting the ESP32

### Connection drops frequently
- **Cause:** Power supply issues or distance
- **Solution:**
  - Use a quality USB cable/power supply
  - Stay within ~10 meters of the paired device
  - Avoid physical obstacles

### "SENT: Press" appears but no key on device
- **Cause:** BLE connected but host not accepting input
- **Solution:**
  - Check device's Bluetooth settings (may need to "trust" the keyboard)
  - Try unpairing and re-pairing
  - Some devices require additional permissions for BLE keyboards

## How It Works

### BLE HID Protocol

The ESP32 advertises itself as a **HID (Human Interface Device) Keyboard** using the Bluetooth Low Energy protocol. When a device pairs with it, the ESP32 can send standard USB HID keyboard reports.

### Keyboard Report Structure

Each keystroke is sent as an 8-byte report:
```
Byte 0: Modifiers (Ctrl, Shift, Alt, GUI) - not used
Byte 1: Reserved (always 0)
Bytes 2-7: Up to 6 key codes (6-key rollover)
```

### Key Press Flow

1. **Button pressed** → GPIO reads LOW (INPUT_PULLUP)
2. **Debounce timer** → Wait 25ms for stable state
3. **Convert to HID code** → 'a' = 0x04, 'b' = 0x05, etc.
4. **Add to report** → Insert into first empty slot in `keys[6]` array
5. **Send BLE notification** → Transmit 8-byte report to host
6. **Host processes** → Character appears on screen

### Key Release Flow

1. **Button released** → GPIO reads HIGH
2. **Debounce timer** → Wait 25ms
3. **Find HID code** → Search `keys[6]` array
4. **Clear slot** → Set to 0
5. **Send BLE notification** → Update host
6. **Host processes** → Key no longer held

## Extending the Code

### Add Special Keys

To support Enter, Escape, arrows, etc., modify `keyToHidCode()`:

```cpp
uint8_t keyToHidCode(uint8_t key)
{
  if (key >= 'a' && key <= 'z')
    return 0x04 + (key - 'a');
  
  // Add special keys
  if (key == '\n') return 0x28;  // Enter
  if (key == '\t') return 0x2B;  // Tab
  if (key == ' ')  return 0x2C;  // Space
  // ... add more as needed
  
  return 0;
}
```

Then update `buttonKeys[]`:
```cpp
static const uint8_t buttonKeys[BUTTON_COUNT] = {'a', '\n', '\t', ' '};
```

### Add Modifier Keys

To send Ctrl, Shift, Alt, modify the `modifiers` byte in `pressKey()`:

```cpp
keyReport.modifiers = 0x01;  // Left Ctrl
keyReport.modifiers = 0x02;  // Left Shift
keyReport.modifiers = 0x04;  // Left Alt
```

## Technical Specifications

- **Board:** Lolin ESP32-C3 Pico
- **Chip:** ESP32-C3 (RISC-V single-core @ 160MHz)
- **Protocol:** Bluetooth Low Energy (BLE) 5.0
- **Profile:** HID (Human Interface Device)
- **Report Type:** Keyboard (Boot Protocol)
- **Max Simultaneous Keys:** 6 (standard USB HID)
- **Supported Characters:** Lowercase a-z
- **Debounce Time:** 25ms (configurable)
- **Power Consumption:** ~80mA active, ~10mA idle (ESP32-C3)
- **Range:** ~10 meters (line of sight)
- **Flash:** 4MB
- **RAM:** 400KB SRAM

## License

This code is provided as-is for educational and personal use.

## Credits

- **USB HID Specification:** USB Implementers Forum
- **ESP32 BLE Stack:** Espressif Systems
- **Arduino ESP32 Core:** Espressif & community contributors

## Support

For issues or questions:
1. Check Serial Monitor output (115200 baud)
2. Verify wiring matches the diagram
3. Ensure ESP32 board is properly selected in Arduino IDE
4. Try different GPIO pins if experiencing boot issues

---

**Version:** 1.0  
**Last Updated:** March 2026  
**Board:** Lolin ESP32-C3 Pico  
**Compatible with:** ESP32 Arduino Core 2.0.0+ (tested on 3.3.7)
