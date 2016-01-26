#include <Arduino.h>
#include "timer.h"



#define CLKDIV1    1
#define CLKDIV8    2
#define CLKDIV64   3
#define CLKDIV256  4
#define CLKDIV1024 5

byte Timer1Prescaler;
/*
CS12 CS11 CS10 Description
0 0 0 No clock source (Timer/Counter stopped).
0 0 1 clkI/O/1 (No prescaling)
0 1 0 clkI/O/8 (From prescaler)
0 1 1 clkI/O/64 (From prescaler)
1 0 0 clkI/O/256 (From prescaler)
1 0 1 clkI/O/1024 (From prescaler)
1 1 0 External clock source on T1 pin. Clock on falling edge.
1 1 1 External clock source on T1 pin. Clock on rising edge.
*/
//init a compare wait of micros on timer 1
void Timer1Init(word micros )
{
    // initialize Timer1
    cli();             // disable global interrupts
    TIFR1  = 0xFF;     //clear interrupt
    TCCR1B = 0;        //stoped timer
    TCNT1  = 0;
    TCCR1A = 0;        // set entire TCCR1A register to 0

    // turn on CTC mode:the counter is cleared when (TCNT1) matches the OCR1A
    TCCR1B |= (1 << WGM12);

    // enable timer compare interrupt:
    //TIMSK1 |= (1 << OCIE1A);
    
    // set compare match register to desired timer count:
    OCR1A = micros*2;
         
    // Set CS11 bit  prescaler to 1/8. increments every 0.5 micro seconds
    //start timer
    TCCR1B = (1 << CS11);

//    TCCR1B |=  5 ;  // 1 0 1 clkI/O/1024 (From prescaler) = 64 ms

    // enable global interrupts:
    sei();
}

//wait interupt on timer 1 by pooling
void Timer1Wait()
{
  //wait interrupt compare flag
  while(TIFR1==0);
  TIFR1  = 0xFF;     //clear interrupt
}

void DelayMicros(word micros)
{
  Timer1Init( micros ) ;
  Timer1Wait() ;
}

