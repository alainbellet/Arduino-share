# ESP32-C3 Web Bluetooth Button Example

A simple Web Bluetooth example that monitors a button connected to an ESP32-C3 and displays its state in a web browser.

## Hardware Setup

- **ESP32-C3** board
- **3 Buttons** connected to GPIO pins (with internal pull-up enabled):
  - **Button 1**: GPIO 0
  - **Button 2**: GPIO 1
  - **Button 3**: GPIO 2
- Connect one side of each button to its respective GPIO pin
- Connect the other side of each button to GND

## Arduino Setup

1. Open `WEB-BLE-SIMPLE.ino` in Arduino IDE
2. Install the ESP32 board support if not already installed:
   - Go to File → Preferences
   - Add to Additional Board Manager URLs: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Go to Tools → Board → Boards Manager
   - Search for "esp32" and install
3. Select your board: Tools → Board → ESP32 Arduino → ESP32C3 Dev Module
4. Upload the sketch to your ESP32-C3

## Web Interface Setup

1. Open `index.html` in a Web Bluetooth compatible browser:
   - **Chrome** (recommended)
   - **Edge**
   - **Opera**
   
   ⚠️ **Note**: Web Bluetooth requires HTTPS or localhost. To test locally:
   - Use a local web server (e.g., `python3 -m http.server 8000`)
   - Or use the Live Server extension in VS Code

2. Click "Connect to ESP32-C3" button
3. Select "ESP32-C3-Button" from the device list
4. Press the physical button to see state changes in real-time

## How It Works

### Arduino Side
- Creates a BLE server with a custom service and 3 characteristics (one per button)
- Monitors button states on GPIO 0, 1, and 2
- Sends notifications when any button state changes (pressed/released)
- Uses internal pull-up resistors (button pressed = LOW)

### Web Side
- Uses Web Bluetooth API to connect to ESP32-C3
- Subscribes to all 3 button state notifications
- Displays each button state with visual feedback in a grid layout
- Logs all events with timestamps

## UUIDs Used

- **Service UUID**: `12345678-1234-1234-1234-123456789012`
- **Characteristic UUID 1** (Button 1): `87654321-4321-4321-4321-210987654321`
- **Characteristic UUID 2** (Button 2): `87654321-4321-4321-4321-210987654322`
- **Characteristic UUID 3** (Button 3): `87654321-4321-4321-4321-210987654323`

## Next Steps for Game Development

This example provides a foundation for building browser-based games controlled by ESP32:

- Add more buttons/sensors
- Implement game logic in JavaScript
- Use Canvas API or game frameworks (Phaser, PixiJS)
- Add multiple characteristics for different controls
- Implement bidirectional communication (send data to ESP32)

## Troubleshooting

- **Can't find device**: Make sure ESP32 is powered and running the sketch
- **Connection fails**: Reset the ESP32 and try again
- **Web Bluetooth not available**: Use Chrome/Edge and ensure you're on HTTPS or localhost
- **Button not responding**: Check GPIO 0/1/2 connections and ensure buttons are wired to GND
