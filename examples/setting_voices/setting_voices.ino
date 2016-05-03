// Arduino synth library basic example

//Hardware connections:

//                    +10µF 
//PIN 11 ---[ 1k ]--+---||--->> Audio out
//                  |
//                 === 10nF
//                  |
//                 GND

// DZL 2014
// HTTP://dzlsevilgeniuslair.blogspot.dk
// HTTP://illutron.dk
// Modified by manticore to support attiny85/84
// part of cocomaketinyriot

#include <TeenySynth.h>
#include <util/delay.h>

TeenySynth synth;    //-Make a synth

void setup() {

  synth.begin();  //-Start it up

  //Set up the voices:
  //setupVoice( voice[0-3] , waveform[SINE,TRIANGLE,SQUARE,SAW,RAMP,NOISE] , pitch[0-127], envelope[ENVELOPE0-ENVELOPE3], length[0-127], mod[0-127, 64=no mod])

  synth.setupVoice(0,SINE,60,ENVELOPE1,60,64);
  synth.setupVoice(1,RAMP,0,ENVELOPE3,64,64);
  synth.setupVoice(2,TRIANGLE,0,ENVELOPE2 ,70,64);
  synth.setupVoice(3,NOISE,0,ENVELOPE3,20,64);
}

void loop()
{
  //-Plays some notes on the voices:
  
  for(int i=49;i<57;i++)
   {
     synth.mTrigger(0,i);
     synth.mTrigger(1,i);

     _delay_ms(300);

     synth.setLength(3,30);
     synth.mTrigger(3,i);

     _delay_ms(150);

     synth.mTrigger(2,i);
     synth.setLength(3,20);
     synth.mTrigger(3,i);

     _delay_ms(150);
   }
}
