// SparkFun MIDI Sheild and MIDI Breakout test code
// Defines bare-bones routines for sending and receiving MIDI data
// Written 02/16/10

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_CAP1188.h>
#include "Adafruit_GFX.h"
#include <Adafruit_NeoPixel.h>

#define LIGHT_PROTOTYPE

// Reset Pin is used for I2C or SPI
#define CAP1188_RESET  34
// CS pin is used for software or hardware SPI
#define CAP1188_CS  10

// These are defined for software SPI, for hardware SPI, check your
// board's SPI pins in the Arduino documentation
#define CAP1188_MOSI  11
#define CAP1188_MISO  12
#define CAP1188_CLK  13

// defines for MIDI Shield components only
#define KNOB1  0
#define KNOB2  1

#define BUTTON1  2
#define BUTTON2  3
#define BUTTON3  4

#define STAT1  7
#define STAT2  6

#define OFF 1
#define ON 2
#define WAIT 3

byte incomingByte;
byte note;
byte velocity;
int pot;

byte byte1;
byte byte2;
byte byte3;

byte notes[] = {60, 62, 64, 65, 67, 71, 72, 74, 76, 77, 79, 81, 83, 84, 86, 88};

uint16_t lights[4][36];

int action = 2; //1 =note off ; 2=note on ; 3= wait

#define NEO_PIN 24
uint16_t numLeds = 189;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLeds, NEO_PIN, NEO_GRB + NEO_KHZ800);

Adafruit_CAP1188 cap1 = Adafruit_CAP1188();
#ifndef LIGHT_PROTOTYPE
Adafruit_CAP1188 cap2 = Adafruit_CAP1188();
#endif
uint16_t touchBits = 0;

#define COLOUR_PAD 0x110011
#define COLOUR_BACK 0x000005
#define COLOUR_LIT 0x330000

void setup() {


  for (int i = 0; i < 4; i++) {
    for (int x = 0; x < 4; x++) {
      for (int y = 0; y < 9; y++) {
        int index = (y * 4) + x;
        int lednum ;

        if (y % 2) {
          lednum =  (y * 21) +  19 - (i * 5) - x  ;
        } else {
          lednum = ((i * 5) + 1) + x + (y * 21);
        }

        lights[i][index] = lednum;
      }
    }
  }


  strip.begin();
  for (int j = 0; j < numLeds; j++) {
    strip.setPixelColor(j, COLOUR_BACK);
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 36; j++) {

      strip.setPixelColor(lights[i][j], COLOUR_PAD);
    }
  }
  //strip.clear();
  strip.show();

  pinMode(STAT1, OUTPUT);
  pinMode(STAT2, OUTPUT);

  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  pinMode(BUTTON3, INPUT);

  digitalWrite(BUTTON1, HIGH);
  digitalWrite(BUTTON2, HIGH);
  digitalWrite(BUTTON3, HIGH);

  for (int i = 0; i < 10; i++) // flash MIDI Sheild LED's on startup
  {
    digitalWrite(STAT1, HIGH);
    digitalWrite(STAT2, LOW);
    delay(30);
    digitalWrite(STAT1, LOW);
    digitalWrite(STAT2, HIGH);
    delay(30);
  }
  digitalWrite(STAT1, HIGH);
  digitalWrite(STAT2, HIGH);

  //start serial with midi baudrate 31250
  Serial.begin(31250);

  initCap1188();


}

void loop () {
#ifdef LIGHT_PROTOTYPE
  uint16_t touched = cap1.touched();
#else
  uint16_t touched = cap1.touched() | (cap2.touched() << 8);

#endif
  if (touched != touchBits) {
    int pixelspernote = 4;
    int spacing = 1;
    // XOR to see what's changed
    uint16_t changed = touched ^ touchBits;

    for (uint8_t i = 0; i < 16; i++) {
      if (changed & (1 << i)) {
        if (touched & (1 << i)) { //note on
          Midi_Send(0x90, notes[i], 0x45);

          for (int j = 0; j < 36; j++) {
            //strip.setPixelColor(i * (spacing + pixelspernote) + j + spacing, strip.Color(60, 60, 60));
            strip.setPixelColor(lights[i - 4][j], COLOUR_LIT);
          }

        } else { // note off
          Midi_Send(0x80, notes[i], 0x45);
          for (int j = 0; j < 36; j++) {
            //strip.setPixelColor(i * (spacing + pixelspernote) + j + spacing, strip.Color(0, 10, 10));
            strip.setPixelColor(lights[i - 4][j], COLOUR_PAD);
          }
        }
      }
    }

    touchBits = touched;
  }
  strip.show();
  // delay(100);
}

void Midi_Send(byte cmd, byte data1, byte data2) {
  Serial.write(cmd);
  Serial.write(data1);
  Serial.write(data2);
}

void blink() {
  digitalWrite(STAT1, HIGH);
  delay(100);
  digitalWrite(STAT1, LOW);
  delay(100);
}

char button(char button_num)
{
  return (!(digitalRead(button_num)));
}

void initCap1188() {

  // Initialize the sensor, if using i2c you can pass in the i2c address
  if (!cap1.begin(0x28)) {
    // Serial.println("CAP1188 not found");
    while (1);
  }

#ifndef LIGHT_PROTOTYPE
  // Serial.println("CAP1188 found!");
  if (!cap2.begin(0x29)) {
    // Serial.println("CAP1188 not found");
    while (1);
  }
#endif

  // adjust sensitivity
  // 0x00 to 0x70 for max to min sensitivity
  uint8_t reg = cap1.readRegister( 0x1f ) & 0x0f;
  cap1.writeRegister( 0x1f, reg | 0x30 ); // after the | is the level

#ifndef LIGHT_PROTOTYPE
  cap2.writeRegister( 0x1f, reg | 0x40 );
#endif
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
  reg = reg | 0b1000;

  // adjust cycle time
  //reg = reg | 0b0000;

  cap1.writeRegister( 0x24, reg);
#ifndef LIGHT_PROTOTYPE
  cap2.writeRegister( 0x24, reg);
#endif

  //  pinMode(CAP1188_RESET, OUTPUT);
  //  digitalWrite(CAP1188_RESET, LOW);
  //  delay(100);
  //  digitalWrite(CAP1188_RESET, HIGH);
  //  delay(100);
  //  digitalWrite(CAP1188_RESET, LOW);
  //  delay(100);

}
