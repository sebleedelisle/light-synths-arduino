#include <Wire.h>
#include <SPI.h>
#include <Adafruit_CAP1188.h>


Adafruit_CAP1188 cap1 = Adafruit_CAP1188();


void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13,LOW); 
  
  Serial.begin(115200);
  Serial.println("CAP1188 test!");
  
  if (!cap1.begin(0x28)) {
  //if (!cap.begin()) {
    Serial.println("CAP 1 not found");
    digitalWrite(13,HIGH); 
    while (1);
  }

}

void loop(){ 
  
  
}
