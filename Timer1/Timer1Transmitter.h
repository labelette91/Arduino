#ifndef _TIMER1.H
#define _TIMER1.H

class Timer1Transmitter
{

  public:
  word  Buffer[256] ;
  byte  pin;
  word * ptbuf ;
  byte NbTransmit ;
  byte PinDATA ;
  byte ledPin ;

  void SetPin(byte PDATA , byte PLED = 0);
  void Timer1Intr(void );
  void StopTimer1(void );
  void StartTimer1(void );
  void StartTransmission(byte pNbTransmit) ;

};
extern Timer1Transmitter timer1Transmitter ;
#endif
