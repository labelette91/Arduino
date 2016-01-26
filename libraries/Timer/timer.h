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
void Timer1Init(word micros );

//wait interupt on timer 1 by pooling
void Timer1Wait();

void DelayMicros(word micros);


extern byte Timer1Prescaler;

inline void Timer1Stop()
{
    TCCR1B &= 0xF8 ;   //reset CS12..CS10     
}
inline void Timer1SetPrescaler(byte prescaler)
{
  Timer1Prescaler=prescaler;  
}
inline void Timer1Run()
{
  TIFR1 = 0 ; //clear interupt pending
  TCCR1B |= Timer1Prescaler ; //run
}
//clear counter when match
inline void Timer1SetCTCMode()
{
  TCCR1A = 0;      
  TCCR1B = (1 << WGM12);
}
//free run
inline void TimerSetNormalMode()
{
  TCCR1A = 0;      
  TCCR1B = 0 ; //WGM12=0 WGM13=0
}

inline void Timer1Clear()
{
    TCNT1  = 0;
}
inline word Timer1Get()
{
    return TCNT1  ;
}
inline word Timer1GetMicro()
{
  word cnt;
  
    Timer1Stop();
    //if overflow 
    if (TIFR1&TOIE1)
    {
      cnt = 0xFFFF;
      TIFR1 &= ~TOIE1 ;
    }
    else
      cnt= TCNT1 / 2 ; //prescaler 1/8 = 0.5 micros 
    
    return  cnt ;
}
inline void Timer1DesableInterrupt()
{
    TIMSK1 = 0 ;
}
inline void Timer0DesableInterrupt()
{
    TIMSK0 = 0 ;
}

inline void Timer0EnableOverflowInterrupt()
{
    TIMSK0 |= 1 ;
}
inline void Timer1EnableOverflowInterrupt()
{
    TIMSK1 |= 1 ;
}
