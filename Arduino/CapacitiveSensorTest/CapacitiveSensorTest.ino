
#include <SPI.h>
#include <Wire.h>
#include "Adafruit_GFX.h"
#include <CapacitiveSensor.h>
#include <Adafruit_SSD1306.h>


CapacitiveSensor  sensor = CapacitiveSensor(2, 3);       // 10 megohm resistor between pins 4 & 2, pin 2 is sensor pin, add wire, foil
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

int values[128]; 
int index = 0; 

void setup() {

  Serial.begin(115200);
  
  pinMode(A0, OUTPUT); 
  pinMode(A2, OUTPUT); 
  pinMode(A1, INPUT); 
  digitalWrite(A0, LOW); 
  digitalWrite(A2, HIGH);
  
  

  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  display.clearDisplay();
  display.display();
  // display.startscrollleft(0x00, 0x0e);

    
  sensor.set_CS_AutocaL_Millis(0xFFFFFFFF); // disable autocalibration

}

void loop() {
  long sensorReading =  sensor.capacitiveSensorRaw(10);
  Serial.println(analogRead(A1));
  
  int i = index%128;
  values[i] = sensorReading * mapFloat(analogRead(A1), 0, 1024,2, 0.01); 
  index++; 
  
  //for(int i = 0; i<128; i++) { 
    
 //   display.drawLine((i+1)%128, 0, (i+1)%128, display.height()-1, BLACK);
    //display.drawLine(i, display.height()-1, i, display.height()-1-values[i], WHITE);
  
  display.drawPixel(i, display.height()-1-values[i], WHITE);
  i = (i+1)%128; 
  display.drawPixel(i, display.height()-1-values[i], BLACK);
   

//}
  
  
  display.display();
  //delay(16);

}

float mapFloat(float v, float min1, float max1, float min2, float max2) { 
  return( (((v-min1) / (max1-min1)) * (max2 - min2)) + min2); 
}
