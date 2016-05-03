#include <TeenySynth.h>
#include <util/delay.h>
#include <Arduino.h>

TeenySynth synth;    //-Make a synth


int melody[] = {
    NOTE_D4, 0, NOTE_F4, NOTE_D4, 0, NOTE_D4, NOTE_G4, NOTE_D4, NOTE_C4,
    NOTE_D4, 0, NOTE_A4, NOTE_D4, 0, NOTE_D4, NOTE_AS4, NOTE_A4, NOTE_F4,
    NOTE_D4, NOTE_A4, NOTE_D5, NOTE_D4, NOTE_C4, 0, NOTE_C4, NOTE_A3, NOTE_E4, NOTE_D4,
    0
};


static uint16_t toSecondsCounter  = 0;
static uint16_t timeCalibrationCounter  = 0;
unsigned long lastTime = 0;            // variable to store the last time we sent a chord

void setup()
{
    synth.begin();
    synth.setupVoice(0,NOISE,60,ENVELOPE1,80,64);
    synth.setupVoice(1,SQUARE,62,ENVELOPE0,100,64);
    synth.setupVoice(2,NOISE,64,ENVELOPE2,110,64);
    synth.setupVoice(3,SQUARE,67,ENVELOPE0,110,64);
}


void loop()
{

    unsigned long m = synth.millis();

    if (m-lastTime >= 100 ){
           lastTime = synth.millis(); // cant use millis from arduino - iox
           synth.mTrigger(0,melody[timeCalibrationCounter]+20);
           //_delay_ms(5);
           lastTime = synth.millis();
           timeCalibrationCounter++;
           if (timeCalibrationCounter>28) timeCalibrationCounter = 0;
    }

}
