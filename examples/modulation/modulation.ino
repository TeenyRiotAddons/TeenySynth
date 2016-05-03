// Arduino synth library modulation example

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

TeenySynth synth;    //-Make as synth

long t = 0; 
int v = 0; 
 
void setup()
{

  synth.begin();  //-Start it up
  //Set up a voice:
  //setupVoice( voice[0-3] , waveform[SINE,TRIANGLE,SQUARE,SAW,RAMP,NOISE] , pitch[0-127], envelope[ENVELOPE0-ENVELOPE3], length[0-127], mod[0-127, 64=no mod])
  synth.setupVoice(0,SQUARE,90,ENVELOPE0,80,64);
}



void loop()
{
  for(int i=30;i<104;i+=10)
  {
      synth.setMod(0,i);
      synth.trigger(0);
     _delay_ms(1000);
  }
}

int main(void)
{
  //init();
    setup();
    for (;;)
        loop();
    return 0;
}
