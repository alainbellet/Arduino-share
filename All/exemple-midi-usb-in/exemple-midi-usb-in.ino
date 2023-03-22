/* 
* utilise la librairie usb-midi est ses dépendences, bien checker 
* l'import des dépendance quand vous installez la librairie 
* via le library manager
* https://github.com/lathoub/Arduino-USBMIDI

*/

#include <USB-MIDI.h>

USBMIDI_CREATE_DEFAULT_INSTANCE();

using namespace MIDI_NAMESPACE;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  // Listen for MIDI messages on channel 1
  MIDI.begin(1);

  MIDI.setHandleNoteOn(OnNoteOn);
  MIDI.setHandleNoteOff(OnNoteOff);
  MIDI.setHandleControlChange(OnControlChange);
}

void loop() {
  // Listen to incoming notes
  MIDI.read();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

static void OnNoteOn(byte channel, byte note, byte velocity) {
  Serial.print(F("NoteOn  from channel: "));
  Serial.print(channel);
  Serial.print(F(", note: "));
  Serial.print(note);
  Serial.print(F(", velocity: "));
  Serial.println(velocity);
}

static void OnNoteOff(byte channel, byte note, byte velocity) {
  Serial.print(F("NoteOff from channel: "));
  Serial.print(channel);
  Serial.print(F(", note: "));
  Serial.print(note);
  Serial.print(F(", velocity: "));
  Serial.println(velocity);
}

static void OnControlChange(byte channel, byte number, byte value) {
  Serial.print(F("ControlChange from channel: "));
  Serial.print(channel);
  Serial.print(F(", number: "));
  Serial.print(number);
  Serial.print(F(", value: "));
  Serial.println(value);
}
