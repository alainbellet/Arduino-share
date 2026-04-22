/*
  AudioTools PCM5102 (I2S DAC) Player for LOLIN ESP32-S3 Pro

  This sketch plays a WAV file ("/"test.wav") from the onboard
  MicroSD card via an external PCM5102 I2S DAC using Phil Schatzmann's
  AudioTools library.

  Wiring for PCM5102 with minijack output:
  PCM5102   ESP32-S3 Pro
  -------   ------------
  VIN   ->  3.3V 
  GND   ->  GND
  SCK   ->  GND (The PCM5102 generates its own system clock if SCK is grounded)
  BCK   ->  GPIO 16
  LRCK  ->  GPIO 17  (often labeled LCK or WS)
  DIN   ->  GPIO 18

  Wiring for MAX98357 Amplifier:
  PCM5102   ESP32-S3 Pro
  -------   ------------
  VIN   ->  3.3V
  GND   ->  GND
  BCLK  ->  GPIO 16
  LRC   ->  GPIO 17  
  DIN   ->  GPIO 18

  SD Card Pins (Internal to Lolin ESP32-S3 Pro, handled automatically via SPI):
  CS   = 46
  MOSI = 11
  MISO = 13
  SCK  = 12
*/

#include "AudioTools.h"
#include <SD.h>
#include <SPI.h>

File file;
const char *file_name = "/sounds/16-bit-PCM-mono-uncompressed-example.wav";

// I2S Output, Volume Control, Format Converter, and Decoder
I2SStream i2s;
VolumeStream vol(i2s);                // Apply volume control just before I2S
FormatConverterStream converter(vol); // Converts Mono to Stereo before Volume
WAVDecoder decoder;
EncodedAudioStream dec(&converter,
                       &decoder); // Decoder sends audio to converter
StreamCopy copier(dec, file);     // Copier streams data from SD to Decoder

// Customize I2S pins here
#define I2S_BCK_PIN 16
#define I2S_LRCK_PIN 17
#define I2S_DATA_PIN 18

// S3 Pro SD Card CS Pin
#define SD_CS_PIN 46

void setup() {
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);
  delay(2000); // Give serial time to start
  Serial.println("\n--- Starting WAV I2S Player ---");

  // Reconfigure default SPI pins to the Lolin S3 Pro internal SD layout
  // SPI.begin(SCK, MISO, MOSI, SS);
  SPI.begin(12, 13, 11, SD_CS_PIN);

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("ERROR: SD card mount failed! Ensure a card is inserted and "
                   "formatted as FAT32.");
    return;
  }
  Serial.println("SD Card mounted successfully.");

  // Open the audio file
  file = SD.open(file_name);
  if (!file) {
    Serial.printf("ERROR: Failed to open %s. Make sure this file exists on "
                  "your SD card!\n",
                  file_name);
    return;
  }
  Serial.printf("Playing %s...\n", file_name);

  // Setup I2S
  auto config = i2s.defaultConfig(TX_MODE);
  config.pin_bck = I2S_BCK_PIN;
  config.pin_ws = I2S_LRCK_PIN;
  config.pin_data = I2S_DATA_PIN;

  // The WAV decoder will automatically update these values from the file header
  // but it's good practice to provide reasonable defaults:
  config.sample_rate = 44100;
  config.channels = 2;
  config.bits_per_sample = 16;

  i2s.begin(config);

  // Configure target settings for the DAC output (Stereo)
  AudioInfo target_info(config.sample_rate, 2, config.bits_per_sample);
  
  // Set playback volume to 70%
  vol.setVolume(0.7);
  vol.begin(target_info);

  // Force the format converter to ALWAYS output as 2 channels (Stereo)
  // to the volume stream, even if the WAV decoder feeds it Mono!
  converter.begin(target_info);

  // Start the decoding stream
  dec.begin();
}

void loop() {
  // If the file is open and has data, continuously copy it into the I2S decoder
  // stream
  if (file && file.available()) {
    copier.copy();
  }
}
