// SparkFun MIDI Sheild and MIDI Breakout test code
// Defines bare-bones routines for sending and receiving MIDI data
// Written 02/16/10

#define LIGHT_PROTOTYPE
#define USE_OCTOWS
//#define USE_SERIAL_DEBUG

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_CAP1188.h>
#include <Adafruit_GFX.h>
#include <MIDI.h>
#include "Key.h"

unsigned long frameCount = 25500; // arbitrary seed to calculate the three time displacement variables t,t2,t3

const int numOfKeys = 16;
const int ledsPerRow = (numOfKeys * 5) + 1;
const int numLeds = ledsPerRow * 9; // 21*9;
const int ledsPerKey = 4 * 7;
const int ledsPerStrip = 81 * 2;


#ifdef USE_OCTOWS
#include <OctoWS2811.h> // ----------------------------------------------------------------UNCOMMENT

DMAMEM int displayMemory[ledsPerStrip * 6];
int drawingMemory[ledsPerStrip * 6];
const int config = WS2811_GRB | WS2811_400kHz;
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);
#endif
#ifndef USE_OCTOWS
//#include <Adafruit_NeoPixel.h> //------------------------------------------------------------ UNCOMMENT

int ledPin = 2;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLeds, ledPin, NEO_GRB + NEO_KHZ800);
#endif

// Reset Pin is used for I2C or SPI
#define CAP1188_RESET1  22
#define CAP1188_RESET2  17
#define RESET_TOUCH_PIN 23


byte notes[] = {
  60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79, 81, 83, 84, 86, 88
};

uint16_t lights[numOfKeys][ledsPerKey];

Key* keys[numOfKeys];

Adafruit_CAP1188 cap1 = Adafruit_CAP1188(CAP1188_RESET1);
Adafruit_CAP1188 cap2 = Adafruit_CAP1188(CAP1188_RESET2);

void resetCap1188(Adafruit_CAP1188& cap, boolean doReset);
void initCap1188(Adafruit_CAP1188& cap, uint16_t address);

int recalibrateCountdown = 0;

uint16_t touchBits = 0;

int x = 0;
int y = 0;

float brightness = 64;
float targetBrightness = 64;

int currentHue = 330;
int targetHue = 330;
int currentBackgroundHue = 191;
int targetBackgroundHue = 190;
int backgroundColour = 0;

float backgroundBrightness = 42;
float backgroundTargetBrightness = 42;

void setup() {

  initLeds();

  for (int j = 0; j < numLeds; j++) {
    setLedPixel(j, 0x00000);
  }

  showLeds();

  MIDI.begin(1);
  MIDI.setThruFilterMode(Off);
  // how to send all notes off?
  for (int i = 0; i < 0xff; i++) {
    for (int ch = 0; ch < 16; ch++) {
      MIDI.sendNoteOff( i, 0, ch);
    }
  }

  pinMode(13, OUTPUT);
  pinMode(RESET_TOUCH_PIN, INPUT_PULLUP);

  for (int i = 0; i < numOfKeys; i++) {

    keys[i] = new Key(i, notes[i]);
    int index = 0;
    for (int x = 0; x < 4; x++) {
      for (int y = 1; y < 8; y++) {
        //int index = ((y-1) * 4) + x;
        lights[i][index] = getIndexForPos((x + (i * 5)) + 1, y);
        index++;
      }
    }

  }


  // delay(1000);

  initCap1188(cap1, 0x28);
  if (numOfKeys > 8)  initCap1188(cap2, 0x29);

  for (int j = 0; j < numLeds; j++) {
    setLedPixel(j, 0x000033);
  }

  showLeds();
  lightKeys(false);

  Serial.begin(9600);

}


//------------------------------------------LOOP ----------------------------------------


void loop () {

  unsigned long mils = millis();

  checkMidi();

  if (brightness != targetBrightness) {
    brightness += (targetBrightness - brightness) * 0.05;
    if (abs(brightness - targetBrightness) < 0.5) brightness = targetBrightness;
  }
  if (backgroundBrightness != backgroundTargetBrightness) {
    backgroundBrightness += (backgroundTargetBrightness - backgroundBrightness) * 0.05;
    if (abs(backgroundBrightness - backgroundTargetBrightness) < 0.5) backgroundBrightness = backgroundTargetBrightness;
    backgroundColour = hsl(currentBackgroundHue, 90, backgroundBrightness);
  }
  if (currentHue != targetHue) {
    currentHue++;
    if (currentHue > 360) currentHue = 0;
    currentHue = targetHue;
  }
  if (currentBackgroundHue != targetBackgroundHue) {
    currentBackgroundHue++;
    if (currentBackgroundHue >= 360) currentBackgroundHue = 0;
    backgroundColour = hsl(currentBackgroundHue, 90, backgroundBrightness);

  }


  // happy light
  if (mils < 2000) {
    if (mils % 200 < 100) digitalWrite(13, HIGH);
    else digitalWrite(13, LOW);
  }
  else {
    if (mils % 2000 < 1000) digitalWrite(13, HIGH);
    else digitalWrite(13, LOW);
  }

  // check reset button
  if (!digitalRead(RESET_TOUCH_PIN)) {
    // debounce
    delay(100);
    while (!digitalRead(RESET_TOUCH_PIN));
    delay(100);

    recalibrate();
  }

  for (int i = 0; i < numOfKeys; i++) {
    keys[i]->update();
  }



  uint16_t touched;
  if (numOfKeys <= 8) {
    touched = cap1.touched();
  }
  else {
    touched = cap1.touched() | (cap2.touched() << 8);
  }
  if (touched != touchBits) {
    int pixelspernote = 4;
    int spacing = 1;
    // XOR to see what's changed
    uint16_t changed = touched ^ touchBits;

    for (uint8_t i = 0; i < numOfKeys; i++) {
      if (changed & (1 << i)) {

        int keynum = i;

        if (keynum % 8 > 3) keynum -= 4;
        else keynum += 4;

        if (touched & (1 << i)) { //note on

          keys[keynum]->press();

        }
        else { // note off
          keys[keynum]->release();
        }
      }

    }


    touchBits = touched;
  }

  if (recalibrateCountdown <= 0) {

    if (millis() - mils < 10) showPlasma() ;
    lightKeys(true);
  } else {


    clearPixels(0x000000);

    leds.setTextColor((mils % 400 < 200) ? 0x222222 : 0x111111);
    leds.setCursor(1, 1);
    leds.print("RECALIBRATING");

    recalibrateCountdown--;
    if (recalibrateCountdown <= 0) {
      resetCap1188(cap1, true);
      resetCap1188(cap2, true);
      recalibrateCountdown = 0;
    }



  }
  // sparkles
  //for(int i = 0; i<4; i++)
  //setLedPixel(random(numLeds), 0xffffff);

  // light sweep
  //   x++;
  //   if(x>=ledsPerRow) {
  //   y++;
  //   x = 0;
  //   if(y>=9) y = 0;
  //   }
  //   setLedPixel(x,y, 0xffffff);
  //   setLedPixel(getIndexForPos(ledsPerRow-x,y), 0xfffffff);
  // leds.drawCircle(3,3,3,0xff0000);
  // leds.setCursor(x,1);
  // x--;
  // if(x<-100) x = ledsPerRow+40;
  // //leds.setTextSize(4,7);
  // leds.setTextColor(0xffffff);
  // leds.print("LAZER LIGHT SYNTHS ");

  //leds.drawPixel(0,0,0xffff00);
  //check midi


  showLeds();
}





#ifdef USE_OCTOWS
void showLeds() {

  leds.show();


}

#else
void showLeds() {

  strip.show();


}
#endif

void initLeds() {
#ifdef USE_OCTOWS
  leds.begin();
#else
  strip.begin();
#endif
  showLeds();

}



void setLedPixel(int pixelnum, int colour) {
#ifdef USE_OCTOWS
  leds.setPixel(pixelnum, colour);
#else
  strip.setPixelColor(pixelnum, colour);
#endif

}
void setLedPixel(int x, int y, int colour) {
  //setLedPixel(getIndexForPos(x, y), colour);
  leds.drawPixel(x, y, colour);
}

void clearPixels(int colour) {
  for (int i = 0; i < numLeds; i++) {
    leds.setPixel(i, colour);

  }

}

int getIndexForPos(int x, int y) {

  // long hand version :
  //  int index =  y * ledsPerRow;
  //  if(y%2==0) index+=x;  // can optimise by
  //  else {
  //    index+=(ledsPerRow-x-1);
  //  }

  return (y * ledsPerRow) + ((y & 1) ?  (ledsPerRow - x - 1) : x ) ;
}

void lightKeys(boolean foregroundOnly) {



  for (int i = 0; i < numOfKeys; i++) {
    drawKey(i);
  }

  //showLeds();



}



void drawKey(int keynum) {
  Key& key = *keys[keynum];
  for (int xpos = key.x; xpos < key.x + key.w; xpos++) {
    for (int ypos = key.y; ypos < key.y + key.h; ypos++) {


      //setLedPixel(xpos, ypos, key.pressed ? COLOUR_LIT : COLOUR_PAD ) ;
      setLedPixel(xpos, ypos, hsl(currentHue, map(key.brightness, 255, 0, 0, 80), map(key.brightness, 255, 0, brightness, brightness * 0.1f  ))) ;
    }
  }

}

void sendAllNotesOff() {


}




