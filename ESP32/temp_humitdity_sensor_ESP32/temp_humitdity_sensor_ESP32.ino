// Adafruit IO Temperature & Humidity Example
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-temperature-and-humidity
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016-2017 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

#include <WEMOS_SHT3X.h>

SHT3X sht30(0x45);

/************************ Example Starts Here *******************************/
//#include <Adafruit_Sensor.h>
//#include <DHT.h>
//#include <DHT_U.h>



// set up the 'temperature' and 'humidity' feeds
AdafruitIO_Feed *temperature = io.feed("temperature-studio-outside");
AdafruitIO_Feed *humidity = io.feed("humidity_studio-outside");
AdafruitIO_Feed *battery = io.feed("battery_studio-outside");
AdafruitIO_Feed *battery_voltage = io.feed("battery_voltage_studio-outside");

void setup() {

  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while (! Serial);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);


  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  digitalWrite(LED_BUILTIN, LOW);
  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  delay(1000);

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();


  if (sht30.get() == 0) {
    float celsius = sht30.cTemp;

    Serial.print("celsius: ");
    Serial.print(celsius);
    Serial.println("C");

    // save celsius to Adafruit IO
    temperature->save(celsius);

    delay(1000);

    Serial.print("humidity: ");
    Serial.print(sht30.humidity);
    Serial.println("%");

    // save humidity to Adafruit IO
    humidity->save(sht30.humidity);

    // check battery level
    log_battery();

    delay(3000);

    // deepsleep to cool down
    ESP.deepSleep(5 * 60 * 1e6);
  } else {
    
    Serial.println("Error with DHT30 sensor");
    Serial.println("will go to sleep");
    log_battery();
   
    ESP.deepSleep(5 * 60 * 1e6);
  }

}



uint8_t PercentageFromVoltage(float voltage) {
  // LOLIN D32 (no voltage divider need already fitted to board.
  // or NODEMCU ESP32 with 100K+100K voltage divider
  uint8_t percentage;
  percentage = 2808.3808 * pow(voltage, 4) - 43560.9157 * pow(voltage, 3) + 252848.5888 * pow(voltage, 2) - 650767.4615 * voltage + 626532.5703;
  if (voltage > 4.19) percentage = 100;
  else if (voltage <= 3.50) percentage = 0;

  return percentage;
}

void log_battery(){
   // check battery level
    float voltage = analogRead(35) / 4096.0 * 7.23;
    uint8_t percentage = PercentageFromVoltage(voltage);

    Serial.print("Battery: ");
    Serial.print(voltage);
    Serial.println("V");

    Serial.print("Battery: ");
    Serial.print(percentage);
    Serial.println("%");

    battery->save(percentage);
    battery_voltage->save(voltage);
    delay(10000);
}
