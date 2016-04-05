void checkMidi() { 
  
 
  int code;
  //leds.drawPixel(0,0,0xffff00); 
  if (MIDI.read()) {                    // Is there a MIDI message incoming ?
    Serial.println(String(MIDI.getType(), HEX)+' '+String(NoteOn, HEX));
 
    leds.drawPixel(0,0,0x00ff00); 
    switch(MIDI.getType()) {		// Get the type of the message we caught

    case NoteOn:               // If it is a Program Change
      //int data = MIDI.getData1();	// Blink the LED a number of times 
      // correponding to the program number 
      // (0 to 127, it can last a while..)
      //        leds.setCursor(1,1);
      //        leds.setTextColor(0x333333);
      //        leds.print(data);
              leds.drawPixel(0,0,0xff0000); 
      //        
      break;
//
    case ControlChange:
      code = MIDI.getData1(); 
      if(code == 0){ 
        //leds.drawPixel(0,0,0xffff00); 
        targetBrightness = (float)MIDI.getData2() / 1.27f;
//        leds.setCursor(1,1);
//        leds.setTextColor(0x333333);
//        leds.print(targetBrightness);
//      
    }else if(code == 1){ 
        //leds.drawPixel(0,0,0xffff00); 
        backgroundTargetBrightness = (float)MIDI.getData2() / 1.27f/ 1.27f;
//        leds.setCursor(1,1);
//        leds.setTextColor(0x333333);
//        leds.print(backgroundTargetBrightness);
//          leds.print(targetBrightness);
          
      }else if(code == 2){ 
        //leds.drawPixel(0,0,0xffff00); 
        targetHue = round((float)MIDI.getData2() / 127.0f * 359.0f);
        //leds.setCursor(1,1);
        //leds.setTextColor(0x333333);
        //leds.print(backgroundTargetBrightness);
      }else if(code == 3){ 
        //leds.drawPixel(0,0,0xffff00); 
        targetBackgroundHue = round((float)MIDI.getData2() / 127.0f * 359.0f);
        //leds.setCursor(1,1);
        //leds.setTextColor(0x333333);
        //leds.print(backgroundTargetBrightness);
        
      } else if(code ==10) { 
        // touch sensitivity
        int value =   constrain(MIDI.getData2(), 0, 7); 
        sensitivity = value; 
        recalibrate();
        
        
        
      }  else if(code ==11) { 
        // touch sample count
        int value =   constrain(MIDI.getData2(), 0, 4); 
        samples = value; 
        recalibrate();
        
        
        
      } else if(code ==12) { 
        // touch sample time
        int value =   constrain(MIDI.getData2(), 0, 3); 
        sampletime = value; 
        recalibrate();
        
        
        
      }else if(code ==13) { 
        // just recalibrate touches
        recalibrate();
        
        
        
      }
      
      
      break; 
//      // See the online reference for other message types
       default:
         break;
    }
  }  
  
}
