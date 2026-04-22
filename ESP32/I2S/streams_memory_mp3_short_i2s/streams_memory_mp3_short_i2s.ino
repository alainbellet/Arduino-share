/*
To convert mp3 to C array use the terminal xxd tool
1. navigate to your folder where the mp3 is
cd /path/to/folder

2. run this comand with name of mp3 and output .h file
xxd -i piano_middle_C.mp3 > piano_middle_C.h
or use (to avoid step 4 and 5)
{ echo "#pragma once\n"; xxd -i piano_middle_C.mp3 | sed 's/unsigned char/const unsigned char/'; } > piano_middle_C.h

3. copy the file in the folder near your arduino file .ino
4. add #pragma once on top of file
5. add const (before unsigned char)
6. include the file for compilation
#include "piano_middle_C.h"

  Wiring for PCM5102 with minijack output:
  PCM5102   ESP32-S3 Pro
  -------   ------------
  VIN   ->  3.3V 
  GND   ->  GND
  SCK   ->  GND (The PCM5102 generates its own system clock if SCK is grounded, the small jumper near the pin need to be soldered as well as the H3L)
  BCK   ->  GPIO 16
  LRCK  ->  GPIO 17  (often labeled LCK or WS)
  DIN   ->  GPIO 18

  Wiring for MAX98357 Amplifier:
  MAX98357  ESP32-S3 Pro
  -------   ------------
  VIN   ->  3.3V
  GND   ->  GND
  BCLK  ->  GPIO 16
  LRC   ->  GPIO 17  
  DIN   ->  GPIO 18
*/

#include "AudioTools.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"
#include "zero.h"
#include "piano_middle_C.h"

// Customize I2S pins here
#define I2S_BCK_PIN 16
#define I2S_LRCK_PIN 17
#define I2S_DATA_PIN 18

// button to play a sound
#define BUTTON_PIN 0 // on lolin S3 pro, the user button

// 1. Initialize objects without specific data
MemoryStream mp3;               
I2SStream i2s;                 
MP3DecoderHelix helix;
EncodedAudioStream out(&i2s, &helix);  
StreamCopy copier(out, mp3);           

void setup() {
  Serial.begin(115200);
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);
  // define button
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Setup I2S
  auto cfg = i2s.defaultConfig();
  cfg.pin_bck = I2S_BCK_PIN;
  cfg.pin_ws = I2S_LRCK_PIN;
  cfg.pin_data = I2S_DATA_PIN;
  cfg.sample_rate = 44100;
  cfg.channels = 1;
  i2s.begin(cfg);
  
  // Setup the output (this starts the decoder)
  out.begin();

  // 2. Now you can call different sounds whenever you want
  playMP3FromMemory(zero_mp3, zero_mp3_len);
  delay(1000); 
  playMP3FromMemory(piano_middle_C_mp3, piano_middle_C_mp3_len);
}

// 3. The Generic Play Function
void playMP3FromMemory(const unsigned char* data, unsigned int len) {
  Serial.println("Playing audio from memory...");
  
  // The correct method name is setValue()
  // We cast to (uint8_t*) to match the library's expected type
  mp3.setValue((uint8_t*)data, len); 
  
  mp3.begin();    // Reset stream pointer to the start of the new data
  helix.begin();  // Reset the decoder state
  
  copier.copyAll(); // Play the whole file
  
  helix.end();    // Flush the decoder output
}

void loop() {
  // Example: Play 'zero' when a button is pressed (pseudo-code)
  
  if(digitalRead(BUTTON_PIN) == LOW) {
      playMP3FromMemory(zero_mp3, zero_mp3_len);
  }
  
}