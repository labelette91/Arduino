#if defined(ARDUINO) && ARDUINO >= 100
#include <stdio.h>
#include <Arduino.h>
#include <avr/pgmspace.h>
#include <timer.h>
#else
typedef unsigned char boolean;
typedef unsigned char byte;
typedef unsigned short word;
void  cli();
word    TCCR1A ;
word    TCCR1B ;
word    TCNT1    ;            
word    TIMSK1  ; 
word    OCR1A ;
void    sei();
#define WGM12 0
#define CS11 0
#define WGM12 0
#define OCIE1A 0

#endif

#include "Timer1Transmitter.h"


void Timer1Transmitter::SetPin(byte PDATA , byte PLED  )
{
  pin = 0 ;
  ptbuf = Buffer ;
  NbTransmit = 0 ;
  PinDATA    = PDATA ;
  ledPin     = PLED ;
}

void Timer1Transmitter::StopTimer1(void )
{
		//stop timer
    cli();
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0  ;            
    TIMSK1 = 0 ; //disable int
    sei();
}
void Timer1Transmitter::StartTimer1(void )
{
    cli();
    // initialize timer1 
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;

    OCR1A = 100*2 ;           // compare match register 
    TCCR1B |= (1 << WGM12);   // CTC mode
    TCCR1B |= (1 << CS11);    // 8  prescaler : 0.5 us
    TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
    sei();
}


void Timer1Transmitter::Timer1Intr(void)
{
  word time ;
  if (pin==0)
  {
    digitalWrite(PinDATA, 1);
    pin=1;
  }
  else 
  {
    digitalWrite(PinDATA, 0);
    pin=0;
  }
  time = (*ptbuf) ;
  time *= 2 ;       //prescaler = *.5 us
	cli();
  OCR1A = time  ;   // compare match register 
  sei();
  if (ledPin) digitalWrite(ledPin, pin );

  //incremente buffer
  ptbuf = ptbuf+1;

  //si fin 
  if (*ptbuf==0){
    NbTransmit--;
    if(NbTransmit==0)
		  //stop timer
      StopTimer1();
    else
    {
      //retransmit
      ptbuf = Buffer;
      pin=0;
    }
  }

}

void Timer1Transmitter::StartTransmission(byte pNbTransmit) 
{
  ptbuf = Buffer;
  pin=0;
  digitalWrite(PinDATA, 0 );
  NbTransmit = pNbTransmit ;

  StartTimer1();

}

Timer1Transmitter  timer1Transmitter  ;

// timer compare interrupt service routine
ISR(TIMER1_COMPA_vect)          
{
  timer1Transmitter.Timer1Intr();
}
