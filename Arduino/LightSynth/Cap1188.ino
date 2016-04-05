

int hue  = 0; 

uint8_t sensitivity = 0x2; //2  // 0x0 to 0x7 - 7 is least sensitive
uint8_t samples = 0x00;  //0// 0=1, 1=2, 2=4, 3=8, 4=16; 
uint8_t sampletime = 0; //0 // 0 = 320us, 1 = 640us, 2 = 1.28ms, 3 = 2.56ms


void recalibrate() { 
  if(recalibrateCountdown ==0 ) 
     recalibrateCountdown = 300;  
  
}

void initCap1188(Adafruit_CAP1188& cap, uint16_t address) {

  // Initialize the sensor, if using i2c you can pass in the i2c address
  while (!cap.begin(address)) {
    // Serial.println("CAP1188 not found");
     //while (1) {

    if(millis()%1000<600) {
      if(millis()%200<100) digitalWrite(13,HIGH);
      else digitalWrite(13,LOW);
    } 
    else  digitalWrite(13,LOW);

    rainbow(10, 250);
    
    
    //}
  } 
  //else {

    resetCap1188(cap, false); 

//  }
}

void resetCap1188(Adafruit_CAP1188& cap, boolean doReset)  { 





  if(doReset) cap.reset();
  // adjust sensitivity
  // 0x00 to 0x70 for max to min sensitivity

  uint8_t reg = cap.readRegister( 0x1f ) & 0x0f;
  cap.writeRegister( 0x1f, reg | (sensitivity<<4) ); // after the | is the level

  // adjust samples per measurement
  // 0 = 1
  // 1 = 2
  // 2 = 4
  // 3 = 8 (default)
  // 4 = 16
  reg = 0;//cap.readRegister( 0x24 ) & 0x0f;
  reg = reg | samples;

  // adjust sample time
  // 0b00xx : 320us
  // 0b01xx : 640us
  // 0b10xx : 1.28ms (default);
  // 0b11xx : 2.56ms (default);
  reg = reg | (sampletime<<2);

  // adjust cycle time
  //reg = reg | 0b0000;

  cap.writeRegister( 0x24, reg);


}


void rainbow(int phaseShift, int cycleTime)
{
  int  x, y;
   hue++; 
   if (hue>180) hue = 0; 
  wait = cycleTime * 1000 / ledsPerStrip;
  for (color=0; color < 180; color++) {
    digitalWrite(1, HIGH);
    for (x=0; x < ledsPerStrip; x++) {
      for (y=0; y < 8; y++) {
        int index = (hue + x + y*phaseShift/2) % 180;
        leds.setPixel(x + y*ledsPerStrip, hsl(index*2, 100, 5));
      }
    }
    leds.show();
    digitalWrite(1, LOW);
    delayMicroseconds(wait);
  }
}

