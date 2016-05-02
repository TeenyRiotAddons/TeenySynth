#ifndef _SYNTH
#define _SYNTH
//*************************************************************************************
//  Arduino synth V4.1
//  Optimized audio driver, modulation engine, envelope engine.
//
//  Dzl/Illutron 2014
//
//*************************************************************************************
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "tables.h"

/*************************************************
 * Public Constants
 *************************************************/
#define NOTE_B0  0
#define NOTE_C1  1
#define NOTE_CS1 2
#define NOTE_D1  3
#define NOTE_DS1 4
#define NOTE_E1  5
#define NOTE_F1  6
#define NOTE_FS1 7
#define NOTE_G1  8
#define NOTE_GS1 9
#define NOTE_A1  10
#define NOTE_AS1 11
#define NOTE_B1  12
#define NOTE_C2  13
#define NOTE_CS2 14
#define NOTE_D2  15
#define NOTE_DS2 16
#define NOTE_E2  17
#define NOTE_F2  18
#define NOTE_FS2 19
#define NOTE_G2  20
#define NOTE_GS2 21
#define NOTE_A2  22
#define NOTE_AS2 23
#define NOTE_B2  24
#define NOTE_C3  25
#define NOTE_CS3 26
#define NOTE_D3  27
#define NOTE_DS3 28
#define NOTE_E3  29
#define NOTE_F3  30
#define NOTE_FS3 31
#define NOTE_G3  32
#define NOTE_GS3 33
#define NOTE_A3  34
#define NOTE_AS3 35
#define NOTE_B3  36
#define NOTE_C4  37
#define NOTE_CS4 38
#define NOTE_D4  39
#define NOTE_DS4 40
#define NOTE_E4  41
#define NOTE_F4  42
#define NOTE_FS4 43
#define NOTE_G4  44
#define NOTE_GS4 45
#define NOTE_A4  46
#define NOTE_AS4 47
#define NOTE_B4  48
#define NOTE_C5  49
#define NOTE_CS5 50
#define NOTE_D5  51
#define NOTE_DS5 52
#define NOTE_E5  53
#define NOTE_F5  54
#define NOTE_FS5 55
#define NOTE_G5  56
#define NOTE_GS5 57
#define NOTE_A5  58
#define NOTE_AS5 59
#define NOTE_B5  60
#define NOTE_C6  61
#define NOTE_CS6 62
#define NOTE_D6  63
#define NOTE_DS6 64
#define NOTE_E6  65
#define NOTE_F6  66
#define NOTE_FS6 67
#define NOTE_G6  68
#define NOTE_GS6 69
#define NOTE_A6  70
#define NOTE_AS6 71
#define NOTE_B6  72
#define NOTE_C7  73
#define NOTE_CS7 74
#define NOTE_D7  75
#define NOTE_DS7 76
#define NOTE_E7  77
#define NOTE_F7  78
#define NOTE_FS7 79
#define NOTE_G7  80
#define NOTE_GS7 81
#define NOTE_A7  82
#define NOTE_AS7 83
#define NOTE_B7  84
#define NOTE_C8  85
#define NOTE_CS8 86
#define NOTE_D8  87
#define NOTE_DS8 88



#define DIFF 1
#define CHA 2
#define CHB 3

#define SINE     0
#define TRIANGLE 1
#define SQUARE   2
#define SAW      3
#define RAMP     4
#define NOISE    5

#define ENVELOPE0 0
#define ENVELOPE1 1
#define ENVELOPE2 2
#define ENVELOPE3 3

#define FS 20000.0                                              //-Sample rate (NOTE: must match tables.h)
#define CPU_CLOCK 16500000.0

#define SET(x,y) (x |=(1<<y))		        		//-Bit set/clear macros
#define CLR(x,y) (x &= (~(1<<y)))       			// |
#define CHK(x,y) (x & (1<<y))           			// |
#define TOG(x,y) (x^=(1<<y))            			//-+

volatile unsigned int PCW[4] = {
  0, 0, 0, 0};			//-Wave phase accumolators
volatile unsigned int FTW[4] = {
  1000, 200, 300, 400};           //-Wave frequency tuning words
volatile unsigned char AMP[4] = {
  255, 255, 255, 255};           //-Wave amplitudes [0-255]
volatile unsigned int PITCH[4] = {
  500, 500, 500, 500};          //-Voice pitch
volatile int MOD[4] = {
  20, 0, 64, 127};                         //-Voice envelope modulation [0-1023 512=no mod. <512 pitch down >512 pitch up]
volatile unsigned int wavs[4];                                  //-Wave table selector [address of wave in memory]
volatile unsigned int envs[4];                                  //-Envelopte selector [address of envelope in memory]
volatile unsigned int EPCW[4] = {
  0x8000, 0x8000, 0x8000, 0x8000}; //-Envelope phase accumolator
volatile unsigned int EFTW[4] = {
  10, 10, 10, 10};               //-Envelope speed tuning word
volatile unsigned char divider = 4;                             //-Sample rate decimator for envelope
volatile unsigned int tim = 0;
volatile unsigned char tik = 0;
volatile unsigned char output_mode;


volatile unsigned long millis_timer_millis = 0;
uint8_t millis_subtimer = 0;

//*********************************************************************************************
//  Audio driver interrupt
//*********************************************************************************************
#ifdef __AVR_ATtinyX5__

SIGNAL(TIMER1_COMPA_vect)
{

    unsigned long m = millis_timer_millis;

    millis_subtimer++;
    if (millis_subtimer > 19) {
        millis_subtimer = 0;
        m += 1;
        millis_timer_millis = m;
    }

  //-------------------------------
  // Time division
  //-------------------------------
  divider++;
  if(!(divider&=0x03))
    tik=1;

  //-------------------------------
  // Volume envelope generator
  //-------------------------------

  if (!(((unsigned char*)&EPCW[divider])[1]&0x80))
    AMP[divider] = pgm_read_byte(envs[divider] + (((unsigned char*)&(EPCW[divider]+=EFTW[divider]))[1]));
  else
    AMP[divider] = 0;

  //-------------------------------
  //  Synthesizer/audio mixer
  //-------------------------------

  OCR0A = 127 +
    ((
  (((signed char)pgm_read_byte(wavs[0] + ((unsigned char *)&(PCW[0] += FTW[0]))[1]) * AMP[0]) >> 8) +
    (((signed char)pgm_read_byte(wavs[1] + ((unsigned char *)&(PCW[1] += FTW[1]))[1]) * AMP[1]) >> 8) +
    (((signed char)pgm_read_byte(wavs[2] + ((unsigned char *)&(PCW[2] += FTW[2]))[1]) * AMP[2]) >> 8) +
    (((signed char)pgm_read_byte(wavs[3] + ((unsigned char *)&(PCW[3] += FTW[3]))[1]) * AMP[3]) >> 8)
    ) >> 2);

  //************************************************
  //  Modulation engine
  //************************************************
  //  FTW[divider] = PITCH[divider] + (int)   (((PITCH[divider]/64)*(EPCW[divider]/64)) /128)*MOD[divider];
  FTW[divider] = PITCH[divider] + (int)   (((PITCH[divider]>>6)*(EPCW[divider]>>6))/128)*MOD[divider];
    tim++;
}

#endif


#ifdef ARDUINO_AVR_ATTINYX4


SIGNAL(TIM0_COMPA_vect)
{

    unsigned long m = millis_timer_millis;

    millis_subtimer++;
    if (millis_subtimer > 19) {
        millis_subtimer = 0;
        m += 1;
        millis_timer_millis = m;
    }

  //-------------------------------
  // Time division
  //-------------------------------
  divider++;
  if(!(divider&=0x03))
    tik=1;

  //-------------------------------
  // Volume envelope generator
  //-------------------------------

  if (!(((unsigned char*)&EPCW[divider])[1]&0x80))
    AMP[divider] = pgm_read_byte(envs[divider] + (((unsigned char*)&(EPCW[divider]+=EFTW[divider]))[1]));
  else
    AMP[divider] = 0;

  //-------------------------------
  //  Synthesizer/audio mixer
  //-------------------------------

  OCR1B = 127 +
    ((
  (((signed char)pgm_read_byte(wavs[0] + ((unsigned char *)&(PCW[0] += FTW[0]))[1]) * AMP[0]) >> 8) +
    (((signed char)pgm_read_byte(wavs[1] + ((unsigned char *)&(PCW[1] += FTW[1]))[1]) * AMP[1]) >> 8) +
    (((signed char)pgm_read_byte(wavs[2] + ((unsigned char *)&(PCW[2] += FTW[2]))[1]) * AMP[2]) >> 8) +
    (((signed char)pgm_read_byte(wavs[3] + ((unsigned char *)&(PCW[3] += FTW[3]))[1]) * AMP[3]) >> 8)
    ) >> 2);

  //************************************************
  //  Modulation engine
  //************************************************
  //  FTW[divider] = PITCH[divider] + (int)   (((PITCH[divider]/64)*(EPCW[divider]/64)) /128)*MOD[divider];
  FTW[divider] = PITCH[divider] + (int)   (((PITCH[divider]>>6)*(EPCW[divider]>>6))/128)*MOD[divider];
    tim++;
}

#endif

class TeenySynth
{
private:

public:

  TeenySynth()
  {
  }

  //*********************************************************************
  //  Startup default
  //*********************************************************************
  unsigned long millis()
  {
    unsigned long m;
    //uint8_t oldSREG = SREG;

    // disable interrupts while we read millis_timer_millis or we might get an
    // inconsistent value (e.g. in the middle of a write to millis_timer_millis)
    //cli();
    m = millis_timer_millis;
    //SREG = oldSREG;

    return m;
  }


  void begin()
  {
//    output_mode=CHA;
//    TCCR1A = 0x00;                                  //-Start audio interrupt
//    TCCR1B = 0x09;
//    TCCR1C = 0x00;
//    OCR1A=CPU_CLOCK / FS;			    //-Auto sample rate
//    SET(TIMSK1, OCIE1A);                            //-Start audio interrupt
//    sei();                                          //-+

//    TCCR2A = 0x83;                                  //-8 bit audio PWM
//    TCCR2B = 0x01;                                  // |
//    OCR2A = 127;                                    //-+
//    SET(DDRB, 3);				    //-PWM pin

#ifdef __AVR_ATtinyX5__
      TCCR1 |= _BV(CTC1); //clear timer on compare
      TIMSK |= _BV(OCIE1A); //activate compare interruppt
      TCNT1 = 0; //init count
      TCCR1 |= _BV(CS10)|_BV(CS12); // prescale 16
      OCR1C = (CPU_CLOCK/16.0)/FS;
      SET(TIMSK, OCIE1A);                            //-Start audio interrupt
      sei();                                          //-+

      //+PWM SETUP
      TCCR0A |= (1<<WGM00)|(1<<WGM01); //Fast pwm
      //TCCR0A |= (1<<WGM00) ; //Phase correct pwm
      TCCR0A |= (1<<COM0A1); //Clear OC0A/OC0B on Compare Match when up-counting.
      TCCR0B |= (1<<CS00);//no prescale
      //+END PWM

      OCR0A = 127;
      SET(DDRB, PB0); //-PWM pin      
#endif

#ifdef ARDUINO_AVR_ATTINYX4

      //Timer 1 pwm mpde
      TCCR1A = 0;
      TCCR1B = 0;
      TCCR1C = 0;
      TCCR1A |= (0b10 << COM1B0); //compare mode COM1B1, COM1B0
      TCCR1A |= (0b01 << WGM10); //Waveform Generation Modes WGM11, WGM10
      TCCR1B |= (0b01 << WGM12); //Waveform Generation Modes WGM13, WGM12
      TCCR1B |= (0b001 << CS10); //Prescaler CS12, CS11, CS10
      TCCR1B |= (0b11 << ICES1); //ICNC1, ICES1
      OCR1B = 127; // set pwm middle
      SET(DDRA, PA5); //-PWM pin

      //Timer 0
      TCCR0A = 0;
      TCCR0B = 0;
      //Table 11-2. Compare Output Mode, non-PWM Mode
      TCCR0A |= (0b10 << COM0A0); //COM0A1, COM0A0 //Toggle OC0A on Compare Match
      //Table 11-9. Clock Select Bit Description
      //0b000 - No clock source (Timer/Counter stopped)
      //0b001 - clkI/O/(No prescaling)
      //0b010 - clkI/O/8 (From prescaler)
      //0b011 - clkI/O/64 (From prescaler)
      //0b100 - clkI/O/256 (From prescaler)
      //0b101 - clkI/O/1024 (From prescaler)
      //0b110 - External clock source on T0 pin. Clock on falling edge.
      //0b111 - External clock source on T0 pin. Clock on rising edge.
      TCCR0B |= (0b010 << CS00);
      //Table 11-8. Waveform Generation Mode Bit Description
      //0b000 - Normal
      //0b001 - PWM, Phase Correct
      //0b010 - CTC top OCRA
      //0b011 - Fast PWM
      //0b100 - Reserved
      //0b101 - PWM, Phase Correct top OCRA
      //0b110 - Reserved
      //0b111 - Fast PWM top OCRA
      TCCR0A |= (0b00 << WGM00); // WGM01, WGM00
      TCCR0B |= (0b0 << WGM02); // WGM02
      //OCR0A – Output Compare Register A
      OCR0A = 49;
      //11.9.6 TIMSK0 – Timer/Counter 0 Interrupt Mask Register
      TIMSK0 |= (1 << OCIE0A); // Timer/Counter0 Output Compare Match A Interrupt Enable
      sei();

#endif

  }

  //*********************************************************************
  //  Startup fancy selecting varoius output modes
  //*********************************************************************

  void begin(unsigned char d)
  {
//    TCCR1A = 0x00;                                  //-Start audio interrupt
//    TCCR1B = 0x09;
//    TCCR1C = 0x00;
//    OCR1A=CPU_CLOCK / FS;			    //-Auto sample rate

#ifdef __AVR_ATtinyX5__

    TCCR1 |= _BV(CTC1); //clear timer on compare
    TIMSK |= _BV(OCIE1A); //activate compare interruppt
    TCNT1 = 0; //init count
    TCCR1 |= _BV(CS10)|_BV(CS12); // prescale 16
    OCR1C = (CPU_CLOCK/16.0)/FS;
    SET(TIMSK, OCIE1A);                            //-Start audio interrupt
    sei();                                          //-+

#endif

    output_mode=d;

    switch(d)
    {
//    case DIFF:                                        //-Differntial signal on CHA and CHB pins (11,3)
//      TCCR2A = 0xB3;                                  //-8 bit audio PWM
//      TCCR2B = 0x01;                                  // |
//      OCR2A = OCR2B = 127;                            //-+
//      SET(DDRB, 3);				      //-PWM pin
//      SET(DDRD, 3);				      //-PWM pin
//      break;

//    case CHB:                                         //-Single ended signal on CHB pin (3)
//      TCCR2A = 0x23;                                  //-8 bit audio PWM
//      TCCR2B = 0x01;                                  // |
//      OCR2A = OCR2B = 127;                            //-+
//      SET(DDRD, 3);				      //-PWM pin
//      break;

    case CHA:
    default:

      output_mode=CHA;                                //-Single ended signal in CHA pin (11)

      #ifdef __AVR_ATtinyX5__

      //+PWM SOUND OUTPUT
      TCCR0A |= (1<<WGM00)|(1<<WGM01); //Fast pwm
      //TCCR0A |= (1<<WGM00) ; //Phase correct pwm
      TCCR0A |= (1<<COM0A1); //Clear OC0A/OC0B on Compare Match when up-counting.
      TCCR0B |= (1<<CS00);//no prescale
      //+END PWM

      OCR0A = 127;

//      TCCR2A = 0x83;                                  //-8 bit audio PWM
//      TCCR2B = 0x01;                                  // |
//      OCR2A = OCR2B = 127;                            //-+

      SET(DDRB, PB0);				      //-PWM pin
#endif

      break;

    }
  }

  //*********************************************************************
  //  Timing/sequencing functions
  //*********************************************************************

  unsigned char synthTick(void)
  {
    if(tik)
    {
      tik=0;
      return 1;  //-True every 4 samples
    }
    return 0;
  }

  unsigned char voiceFree(unsigned char voice)
  {
    if (!(((unsigned char*)&EPCW[voice])[1]&0x80))
      return 0;
    return 1;
  }


  //*********************************************************************
  //  Setup all voice parameters in MIDI range
  //  voice[0-3],wave[0-6],pitch[0-127],envelope[0-4],length[0-127],mod[0-127:64=no mod]
  //*********************************************************************

  void setupVoice(unsigned char voice, unsigned char wave, unsigned char pitch, unsigned char env, unsigned char length, unsigned int mod)
  {
    setWave(voice,wave);
    setPitch(voice,pitch);
    setEnvelope(voice,env);
    setLength(voice,length);
    setMod(voice,mod);
  }

  //*********************************************************************
  //  Setup wave [0-6]
  //*********************************************************************

  void setWave(unsigned char voice, unsigned char wave)
  {
    switch (wave)
    {
    case TRIANGLE:
      wavs[voice] = (unsigned int)TriangleTable;
      break;
    case SQUARE:
      wavs[voice] = (unsigned int)SquareTable;
      break;
    case SAW:
      wavs[voice] = (unsigned int)SawTable;
      break;
    case RAMP:
      wavs[voice] = (unsigned int)RampTable;
      break;
    case NOISE:
      wavs[voice] = (unsigned int)NoiseTable;
      break;
    default:
      wavs[voice] = (unsigned int)SinTable;
      break;
    }
  }
  //*********************************************************************
  //  Setup Pitch [0-127]
  //*********************************************************************

  void setPitch(unsigned char voice,unsigned char MIDInote)
  {
    PITCH[voice]=pgm_read_word(&PITCHS[MIDInote]);
  }

  //*********************************************************************
  //  Setup Envelope [0-4]
  //*********************************************************************

  void setEnvelope(unsigned char voice, unsigned char env)
  {
    switch (env)
    {
    case 1:
      envs[voice] = (unsigned int)Env0;
      break;
    case 2:
      envs[voice] = (unsigned int)Env1;
      break;
    case 3:
      envs[voice] = (unsigned int)Env2;
      break;
    case 4:
      envs[voice] = (unsigned int)Env3;
      break;
    default:
      envs[voice] = (unsigned int)Env0;
      break;
    }
  }

  //*********************************************************************
  //  Setup Length [0-128]
  //*********************************************************************

  void setLength(unsigned char voice,unsigned char length)
  {
    EFTW[voice]=pgm_read_word(&EFTWS[length]);
  }

  //*********************************************************************
  //  Setup mod
  //*********************************************************************

  void setMod(unsigned char voice,unsigned char mod)
  {
    //    MOD[voice]=(unsigned int)mod*8;//0-1023 512=no mod
    MOD[voice]=(int)mod-64;//0-1023 512=no mod
  }

  //*********************************************************************
  //  Midi trigger
  //*********************************************************************

  void mTrigger(unsigned char voice,unsigned char MIDInote)
  {
    PITCH[voice]=pgm_read_word(&PITCHS[MIDInote]);
    EPCW[voice]=0;
    FTW[divider] = PITCH[voice] + (int)   (((PITCH[voice]>>6)*(EPCW[voice]>>6))/128)*MOD[voice];
  }

  //*********************************************************************
  //  Set frequency direct
  //*********************************************************************

  void setFrequency(unsigned char voice,float f)
  {
    PITCH[voice]=f/(FS/65535.0);

  }

  //*********************************************************************
  //  Set time
  //*********************************************************************

  void setTime(unsigned char voice,float t)
  {
    EFTW[voice]=(1.0/t)/(FS/(32767.5*10.0));//[s];
  }

  //*********************************************************************
  //  Simple trigger
  //*********************************************************************

  void trigger(unsigned char voice)
  {
    EPCW[voice]=0;
    FTW[voice]=PITCH[voice];
    //    FTW[voice]=PITCH[voice]+(PITCH[voice]*(EPCW[voice]/(32767.5*128.0  ))*((int)MOD[voice]-512));
  }

  //*********************************************************************
  //  Suspend/resume synth
  //*********************************************************************

  void suspend()
  {
#ifdef __AVR_ATtinyX5__
    CLR(TIMSK, OCIE1A);                            //-Stop audio interrupt
#endif
#ifdef __AVR_ATtinyX4__
    CLR(TIMSK1, OCIE1A);                            //-Stop audio interrupt
#endif
  }
  void resume()
  {
#ifdef __AVR_ATtinyX5__
    SET(TIMSK, OCIE1A);                            //-Start audio interrupt
#endif
#ifdef __AVR_ATtinyX4__
    SET(TIMSK1, OCIE1A);                            //-Start audio interrupt
#endif

  }

};

#endif













