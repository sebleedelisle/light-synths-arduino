#include <MIDI.h>
/*
  MIDI Input tutorial
  by Franky
  28/07/2009
  
  NOTE: for easier MIDI input reading, 
  take a look a the Callbacks example.
  
*/

#define LED 13   		// LED pin on Arduino board

void BlinkLed(byte num) { 	// Basic blink function
  for (byte i=0;i<num;i++) {
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
    delay(100);
  }
}


void setup() {
  pinMode(LED, OUTPUT);
  MIDI.begin(4);            	// Launch MIDI with default options
BlinkLed(5);				// (input channel is default set to 1)

Serial.begin(9600); 
}

void loop() {
  if (MIDI.read()) {   
BlinkLed(1);
Serial.println(String(MIDI.getType(), HEX)+' '+String(NoteOn, HEX));
    // Is there a MIDI message incoming ?
    switch(MIDI.getType()) {		// Get the type of the message we caught
      case NoteOn:               // If it is a Program Change
	BlinkLed(1);	// Blink the LED a number of times 
					// correponding to the program number 
					// (0 to 127, it can last a while..)
        break;
      // See the online reference for other message types
      default:
        break;
    }
  } else { 
    Serial.println("NO MIDI :( ");
  }
  
  MIDI.sendNoteOn(60,100,1); 
   delay(50); 
  MIDI.sendNoteOff(60,0,1); 
 
   
}
