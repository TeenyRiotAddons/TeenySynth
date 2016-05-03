// Arduino synth library Output mode example.

// The synth can output audio in three modes on two different pins:

// OUTA = Arduino PIN 11
// OUTB = Arduino PIN 3

// or

// Differntial on PINA,PINB

//Hardware connection(s):

//                           +10µF 
//PIN 11 (OUTA)---[ 1k ]--+---||--->> Audio out
//                        |
//                       === 10nF
//                        |
//                       GND

//                           +10µF 
//PIN 3 (OUTB) ---[ 1k ]--+---||--->> Audio out
//                        |
//                       === 10nF
//                        |
//                       GND

// Differential mode gives twice the voltage swing and may be better for driving piezos or speakers directly.
// Here are some examples:

// PIN 11 (OUTA) ------- Speaker/Piezo +

// PIN 3 (OUTB)  ------- Speaker/Piezo -

// Or better with a low pass filter

// PIN 11 (OUTA) ----+
//                   |
//                    )
//                    ) 10mH inductor
//                    )
//                   |
//                   +---- Speaker/Piezo +
//                   |
//                  === 1µF
//                   |
//PIN 3 (OUTB)  -----+---- Speaker/Piezo -




#include "TeenySynth.h"
TeenySynth synth;

void setup() {

  //Uncomment desired:
  
  synth.begin();        //Default OUTA
  //edgar.begin(CHA);     //CHA (same as default)
  //edgar.begin(CHB);     //CHB
  //edgar.begin(DIFF);    //Differential
  
  //  Set up the voices
  synth.setupVoice(0,SINE,60,ENVELOPE0,90,64);
  synth.setupVoice(1,SINE,62,ENVELOPE1,90,64);
  synth.setupVoice(2,SINE,64,ENVELOPE2,90,64);
  synth.setupVoice(3,SINE,67,ENVELOPE0,90,64);


  synth.begin();    //-Default OUTA
  //-Play a little gong
  synth.trigger(0);
  _delay_ms(1000);
  synth.trigger(1);
  _delay_ms(1000);
  synth.trigger(2);
  _delay_ms(1000);
  synth.trigger(3);
  _delay_ms(1000);


}
void loop() 
{
}






