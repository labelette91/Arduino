#include <HomeEasyTransmitter.h>
/*
* timer and interrupts
* Timer1 overflow interrupt example
* more infos: https://oscarliang.com
*/

#define ledPin 13
byte counter ;
#define PDATA 3 //pin for data input/output
#define PCLK  0 //pin for clk  input/output


word  Buffer[256] ;
byte  pin;

word * ptbuf ;

void setup ()
{
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW );
  pinMode(PDATA, OUTPUT);
  Serial.begin(38400);
  

  delay(100);

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 000  ;            
  TCCR1B |= (1 << CS11);    // 8  prescaler : 0.5 us

}


#define GET_TIMER1(value)   cli();  value = TCNT1;   sei();

void loop()
{
  pin=0;
	word T1, T2;

  digitalWrite(PDATA, 0 );

  delay(1000);
	
	cli();
	T1 = TCNT1;
	sei();

	delayMicroseconds(300);

	cli();
	T2 = TCNT1;
	sei();

//	Serial.println( (T2 - T1 )/2  );
  if (T2 < T1 ){
  
  Serial.println( (T1 - T2 )  );
  Serial.println( (T2 - T1 )   );
  }
  


}

