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

HomeEasyTransmitter easy(PDATA,0,0);

word  Buffer[256] ;
byte  pin;

word * ptbuf ;


void printBuffer()
{
  word  i=0 ;

  while(Buffer[i]!=0)
  {
    if ( (i%4)==0 ) {
      Serial.println();
      Serial.print(i/4);
      Serial.print(':' ) ;
      
    }
    Serial.print(Buffer[i] );
    Serial.print(' ' ) ;
    i++;
    
  }

}

void setupCTC ()
{
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW );
  pinMode(PDATA, OUTPUT);
  Serial.begin(38400);
  
  easy.SetTransmitBuffer(Buffer,1,0x555555,1);

  delay(100);

	printBuffer();

}

ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
  word time ;
  if (pin==0)
  {
    digitalWrite(PDATA, 1);
    pin=1;
  }
  else 
  {
    digitalWrite(PDATA, 0);
    pin=0;
  }
  time = (*ptbuf) ;
  time *= 2 ;
	OCR1A = time  ;           // compare match register 

  digitalWrite(ledPin, digitalRead(ledPin) ^ 1);

  ptbuf = ptbuf+1;
  if (*ptbuf==0){
		//stop timer
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0  ;            
    TIMSK1 = 0 ;
    
  }

}

void loopCTC()
{
  ptbuf = Buffer;
  pin=0;
  digitalWrite(PDATA, 0 );

  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 300*2 ;           // compare match register 
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS11);    // 8  prescaler : 0.5 us
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts



  delay(1000);

}

#define MICROS(delay)(65536 - (delay)*2)
//overflow
void setupOVF()
{
  word  i=0 ;
  word periode = 32000 ; //32ms
  Serial.begin(38400);

  pinMode(ledPin, OUTPUT);
  pinMode(PDATA, OUTPUT);

  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  TCNT1 = 65536 - periode*2 ;            
  TCCR1B |= (1 << CS11);    // 8  prescaler : 0.5 us
//  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt

  interrupts();             // enable all interrupts

  easy.SetTransmitBuffer(Buffer,1,0x555555,1);

  delay(100);

}

ISR(TIMER1_OVF_vect)        // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
  if (pin==0)
  {
    digitalWrite(PDATA, 1);
    pin=1;
  }
  else 
  {
    digitalWrite(PDATA, 0);
    pin=0;
  }
  TCNT1 = MICROS(*ptbuf);            // preload timer
  digitalWrite(ledPin, digitalRead(ledPin) ^ 1);

  ptbuf = ptbuf+1;
  if (*ptbuf==0){
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0  ;            
    TIMSK1 = 0 ;
    
  }
  
}



void loopOVF()
{
//  easy.setSwitch(  true, 0x55555555 , 1 ) ;
  ptbuf = Buffer;
  pin=0;
  digitalWrite(PDATA, 0 );

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 65000  ;            
  TCCR1B |= (1 << CS11);    // 8  prescaler : 0.5 us
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt

  delay(1000);
  // your program here...
}

void setup()
{
	setupCTC() ;
//	setupOVF() ;

}
void loop()
{
	loopCTC() ;
//	loopOVF() ;

}