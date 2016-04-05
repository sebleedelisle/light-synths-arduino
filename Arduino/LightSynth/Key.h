
class Key { 
public :

  int index, midiNote; 

  int x, y, w, h; 
  boolean pressed; 
  int brightness; // 0 to 255
  int targetBrightness; 

  int pixels[7*4*6]; // assumes a 7 x 4 shape

  Key(int _index, int _midinote){

    index = _index; 
    midiNote = _midinote; 
   // colour = _colour; 

    pressed = false; 
    x = index*5+1; 
    y = 1; 
    w = 4; 
    h = 7; 
    brightness = 0; 
    targetBrightness = 0; 

  };


  void update() { 
    if(!pressed) {
      //brightness += ((0-brightness)*0.5); 
      //if(brightness<0) brightness = 0;  
    } else {
     // brightness = 255; 
      //targetBrightness = 100; 
    }
    
    if(targetBrightness>brightness) brightness = targetBrightness; 
    else brightness += (targetBrightness-brightness)*0.1; 
  }
  boolean press() { 
    if(pressed) return false; 
    MIDI.sendNoteOn( midiNote, 0xaa,1);
      brightness = 255; 
      targetBrightness = 100; 

    pressed = true; 
    return true; 

  }


  boolean release() { 
    if(!pressed) return false; 

    MIDI.sendNoteOff( midiNote, 0,1);
    targetBrightness = 0;
    pressed = false; 
    return true; 

  }


};



