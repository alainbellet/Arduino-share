
// WS2812:
#include <Adafruit_NeoPixel.h>
#define BRIGHTNESS  50
#define NUMPIXELS 2
// GPIO 23, as default.
#define LED_PIN   23

Adafruit_NeoPixel matrix = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void InitLED(){
  matrix.setBrightness(BRIGHTNESS);
  matrix.begin();
  matrix.show();
}

void LEDSetSingle(uint16_t LEDnum, uint32_t c)
{
  matrix.setPixelColor(LEDnum, c);
  matrix.show();
}

void LEDOff(){
  LEDSetSingle(0, matrix.Color(0, 0, 0));
  LEDSetSingle(1, matrix.Color(0, 0, 0));
}

void RGBcolor(byte Rinput, byte Ginput, byte Binput){
  LEDSetSingle(0, matrix.Color(Rinput, Ginput, Binput));
  LEDSetSingle(1, matrix.Color(Rinput, Ginput, Binput));
}