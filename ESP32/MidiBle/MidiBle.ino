#include <BLEMIDI_Transport.h>

#include <hardware/BLEMIDI_ESP32_NimBLE.h>
//#include <hardware/BLEMIDI_ESP32.h>
//#include <hardware/BLEMIDI_nRF52.h>
//#include <hardware/BLEMIDI_ArduinoBLE.h>

BLEMIDI_CREATE_INSTANCE("esp32", MIDI)

unsigned long t0 = millis();
bool isConnected = false;
int counter = 0;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void setup()
{
  MIDI.begin();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  BLEMIDI.setHandleConnected(OnConnected);
  BLEMIDI.setHandleDisconnected(OnDisconnected);

  MIDI.setHandleNoteOn(OnNoteOn);
  MIDI.setHandleNoteOff(OnNoteOff);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loop()
{
  MIDI.read();

  if (isConnected && (millis() - t0) > 5000)
  {
    t0 = millis();
    counter  = (counter + 1) % 127; // test variable
    MIDI.sendNoteOn (70, counter, 1); // note 60, velocity 127 on channel 1
    delay(10);

  }
}

// ====================================================================================
// Event handlers for incoming MIDI messages
// ====================================================================================

// -----------------------------------------------------------------------------
// Device connected
// -----------------------------------------------------------------------------
void OnConnected() {
  isConnected = true;
  digitalWrite(LED_BUILTIN, LOW);
}

// -----------------------------------------------------------------------------
// Device disconnected
// -----------------------------------------------------------------------------
void OnDisconnected() {
  isConnected = false;
  digitalWrite(LED_BUILTIN, HIGH);
}

// -----------------------------------------------------------------------------
// Received note on
// -----------------------------------------------------------------------------
void OnNoteOn(byte channel, byte note, byte velocity) {

}

// -----------------------------------------------------------------------------
// Received note off
// -----------------------------------------------------------------------------
void OnNoteOff(byte channel, byte note, byte velocity) {
}
