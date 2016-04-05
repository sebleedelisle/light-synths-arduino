/*************************************************** 
  This is a library for the CAP1188 I2C/SPI 8-chan Capacitive Sensor

  Designed specifically to work with the CAP1188 sensor from Adafruit
  ----> https://www.adafruit.com/products/1602

  These sensors use I2C/SPI to communicate, 2+ pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/
 
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_CAP1188.h>

#include "Adafruit_GFX.h"
#include <Adafruit_NeoPixel.h>

// Reset Pin is used for I2C or SPI
#define CAP1188_RESET  34
// CS pin is used for software or hardware SPI
#define CAP1188_CS  10

// These are defined for software SPI, for hardware SPI, check your 
// board's SPI pins in the Arduino documentation
#define CAP1188_MOSI  11
#define CAP1188_MISO  12
#define CAP1188_CLK  13

// For I2C, connect SDA to your Arduino's SDA pin, SCL to SCL pin
// On UNO/Duemilanove/etc, SDA == Analog 4, SCL == Analog 5
// On Leonardo/Micro, SDA == Digital 2, SCL == Digital 3
// On Mega/ADK/Due, SDA == Digital 20, SCL == Digital 21

// Use I2C, no reset pin!
Adafruit_CAP1188 cap1 = Adafruit_CAP1188();
Adafruit_CAP1188 cap2 = Adafruit_CAP1188();

// Or...Use I2C, with reset pin
//Adafruit_CAP1188 cap = Adafruit_CAP1188(CAP1188_RESET);

// Or... Hardware SPI, CS pin & reset pin 
// Adafruit_CAP1188 cap = Adafruit_CAP1188(CAP1188_CS, CAP1188_RESET);

// Or.. Software SPI: clock, miso, mosi, cs, reset
//Adafruit_CAP1188 cap = Adafruit_CAP1188(CAP1188_CLK, CAP1188_MISO, CAP1188_MOSI, CAP1188_CS, CAP1188_RESET);


#define LED_DATA_PIN 5
uint16_t numLeds = 240;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLeds, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  Serial.begin(9600);
  Serial.println("CAP1188 test!");

  // Initialize the sensor, if using i2c you can pass in the i2c address
  if (!cap1.begin(0x28)) {
  //if (!cap.begin()) {
    Serial.println("CAP 1 not found");
    while (1);
  }
  Serial.println("CAP 1 found!");
  
  if (!cap2.begin(0x29)) {
  //if (!cap.begin()) {
    Serial.println("CAP1188 2 not found");
    while (1);
  }
   Serial.println("CAP 2 found!");
 
  // adjust sensitivity
  // 0x00 to 0x70 for max to min sensitivity
  uint8_t reg = cap1.readRegister( 0x1f ) & 0x0f;
  cap1.writeRegister( 0x1f, reg | 0x00 );
  
  // adjust samples per measurement
  // 0 = 1
  // 1 = 2
  // 2 = 4
  // 3 = 8 (default)
  // 4 = 16 
  reg = 0;//cap.readRegister( 0x24 ) & 0x0f;
  reg = reg | 0x00;
  // adjust sample time 
  // 0b00xx : 320us
  // 0b01xx : 640us
  // 0b10xx : 1.28ms (default); 
  // 0b10xx : 2.56ms (default); 
  reg = reg | 0b0000; 
  
  // adjust cycle time
  reg = reg | 0b0000; 
  
  cap1.writeRegister( 0x24, reg);

  
}

void loop() {
return;
  uint8_t touched = cap1.touched();

  if (touched == 0) {
    // No touch detected
    return;
  }
  
  for (uint8_t i=0; i<8; i++) {
    if (touched & (1 << i)) {
      Serial.print("C"); Serial.print(i+1); Serial.print("\t");
    }
  }
  Serial.println();
  delay(50);
  
  
  for (uint16_t i = 0; i < numLeds; i++) {
    if (millis() % 500 < 250)
      strip.setPixelColor(i, strip.Color(0, 0, 80));
    else
      strip.setPixelColor(i, strip.Color(255, 0, 255));
  }
  
  strip.show(); 
}

