/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;

#define SETBIT(A,B) A|=(1<<B);
#define RESETBIT(A,B) A&=~(1<<B);

#define SETPORT(PORT,BIT,DATA) if (DATA) SETBIT(PORT,BIT) else RESETBIT(PORT,BIT)

//    port D is pins 0 - 7.
//    port B bank is digital pins 8 - 13.
//    port C bank is the analog input pins.
void digitalFastWrite(byte pin , byte data)
{
  
//  if (data)
//	  switch(pin)
//	  {
//	    case  0 :  SETBIT(PORTD,0 ) ;    return;
//	    case  1 :  SETBIT(PORTD,1 ) ;    return;
//	    case  2 :  SETBIT(PORTD,2 ) ;    return;
//	    case  3 :  SETBIT(PORTD,3 ) ;    return;
//	    case  4 :  SETBIT(PORTD,4 ) ;    return;
//	    case  5 :  SETBIT(PORTD,5 ) ;    return;
//	    case  6 :  SETBIT(PORTD,6 ) ;    return;
//	    case  7 :  SETBIT(PORTD,7 ) ;    return;
//	    case  8 :  SETBIT(PORTB,0 ) ;    return;
//	    case  9 :  SETBIT(PORTB,1 ) ;    return;
//	    case 10 :  SETBIT(PORTB,2 ) ;    return;
//	    case 11 :  SETBIT(PORTB,3 ) ;    return;
//	    case 12 :  SETBIT(PORTB,4 ) ;    return;
//	    case 13 :  SETBIT(PORTB,5 ) ;    return;
//	    case 14 :/*SETBIT(PORTB,6 ) ;*/  return;
//	    case 15 :/*SETBIT(PORTB,7 ) ;*/  return;
//	  }
//	 else
//	  switch(pin)
//	  {
//	    case  0 :  RESETBIT(PORTD,0 ) ;    return;
//	    case  1 :  RESETBIT(PORTD,1 ) ;    return;
//	    case  2 :  RESETBIT(PORTD,2 ) ;    return;
//	    case  3 :  RESETBIT(PORTD,3 ) ;    return;
//	    case  4 :  RESETBIT(PORTD,4 ) ;    return;
//	    case  5 :  RESETBIT(PORTD,5 ) ;    return;
//	    case  6 :  RESETBIT(PORTD,6 ) ;    return;
//	    case  7 :  RESETBIT(PORTD,7 ) ;    return;
//	    case  8 :  RESETBIT(PORTB,0 ) ;    return;
//	    case  9 :  RESETBIT(PORTB,1 ) ;    return;
//	    case 10 :  RESETBIT(PORTB,2 ) ;    return;
//	    case 11 :  RESETBIT(PORTB,3 ) ;    return;
//	    case 12 :  RESETBIT(PORTB,4 ) ;    return;
//	    case 13 :  RESETBIT(PORTB,5 ) ;    return;
//	    case 14 :/*RESETBIT(PORTB,6 ) ;*/  return;
//	    case 15 :/*RESETBIT(PORTB,7 ) ;*/  return;
//	  }
	 
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega88) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega32U4__) 
	  switch(pin)
	  {
	    case  0 :  SETPORT(PORTD,0 ,data ) ;    return;
	    case  1 :  SETPORT(PORTD,1 ,data ) ;    return;
	    case  2 :  SETPORT(PORTD,2 ,data ) ;    return;
	    case  3 :  SETPORT(PORTD,3 ,data ) ;    return;
	    case  4 :  SETPORT(PORTD,4 ,data ) ;    return;
	    case  5 :  SETPORT(PORTD,5 ,data ) ;    return;
	    case  6 :  SETPORT(PORTD,6 ,data ) ;    return;
	    case  7 :  SETPORT(PORTD,7 ,data ) ;    return;
	    case  8 :  SETPORT(PORTB,0 ,data ) ;    return;
	    case  9 :  SETPORT(PORTB,1 ,data ) ;    return;
	    case 10 :  SETPORT(PORTB,2 ,data ) ;    return;
	    case 11 :  SETPORT(PORTB,3 ,data ) ;    return;
	    case 12 :  SETPORT(PORTB,4 ,data ) ;    return;
	    case 13 :  SETPORT(PORTB,5 ,data ) ;    return;
	    case 14 :/*SETPORT(PORTB,6 ,data ) ;*/  return;
	    case 15 :/*SETPORT(PORTB,7 ,data ) ;*/  return;
	  }
#endif

#if defined(__AVR_ATmega2560__)
	  switch(pin)
	  {
	    case  0 :  SETPORT(PORTD,0 ,data ) ;    return;
	    case  1 :  SETPORT(PORTD,1 ,data ) ;    return;
	    case  2 :  SETPORT(PORTD,2 ,data ) ;    return;
	    case  3 :  SETPORT(PORTE,5 ,data ) ;    return;
	    case  4 :  SETPORT(PORTD,4 ,data ) ;    return;
	    case  5 :  SETPORT(PORTD,5 ,data ) ;    return;
	    case  6 :  SETPORT(PORTD,6 ,data ) ;    return;
	    case  7 :  SETPORT(PORTD,7 ,data ) ;    return;
	    case  8 :  SETPORT(PORTB,0 ,data ) ;    return;
	    case  9 :  SETPORT(PORTB,1 ,data ) ;    return;
	    case 10 :  SETPORT(PORTB,2 ,data ) ;    return;
	    case 11 :  SETPORT(PORTB,3 ,data ) ;    return;
	    case 12 :  SETPORT(PORTB,4 ,data ) ;    return;
	    case 13 :  SETPORT(PORTB,7 ,data ) ;    return;
	    case 14 :/*SETPORT(PORTB,6 ,data ) ;*/  return;
	    case 15 :/*SETPORT(PORTB,7 ,data ) ;*/  return;
	  }
#endif


  
}

byte pData=3;

// the setup routine runs once when you press reset:
void setup() {                
 // initialize the digital pin as an output.
  pinMode(led, OUTPUT);    
  pinMode(pData, OUTPUT);    
  Serial.begin(38400);
}


// the loop routine runs over and over again forever:
void loop() {
//  digitalFastWrite(led,1);
//  digitalFastWrite(pData,1);
//  delay(1000);
//  digitalFastWrite(led,0);
//  digitalFastWrite(pData,0);
//  delay(1000);

  cli();
  
  digitalFastWrite(pData,1);
  digitalFastWrite(pData,0);
  digitalFastWrite(pData,1);
  digitalFastWrite(pData,0);

  digitalWrite(pData,1);
  digitalWrite(pData,0);
  digitalWrite(pData,1);
  digitalWrite(pData,0);
  sei();  
}
