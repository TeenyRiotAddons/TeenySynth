// Arduino synth library absolute frequency example

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

  synth.begin();                                   //-Start it up
  synth.setupVoice(0,TRIANGLE,60,ENVELOPE1,127,64);  //-Set up voice 0
}

void loop()
{
  synth.setFrequency(0,50.0);
  synth.trigger(0);
  _delay_ms(1000);
  synth.setFrequency(0,60.0);
  synth.trigger(0);
  _delay_ms(1000);
  synth.setFrequency(0,440.0);
  synth.trigger(0);
  _delay_ms(1000);
  synth.setFrequency(0,1000.0);
  synth.trigger(0);
  _delay_ms(1000);
}
