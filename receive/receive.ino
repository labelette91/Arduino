#include <RFM69.h>
#include <SPI.h>
//#include <SPIFlash.h>

#define NODEID      1
#define NETWORKID   100
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define LED         9
#define SERIAL_BAUD 115200

#define PDATA 3 //pin for data input/output
#define PCLK  3 //pin for clk  input/output

RFM69 radio;
//SPIFlash flash(5, 0x120); //EF40 for 16mbit windbond chip

byte          Pin ;
unsigned long pulse ;
word          NbPulse  ;
byte          np;
static unsigned long  last;


long int  LastReceive ;
word    Dt;
byte nb ;
byte nbc ;
unsigned long T ;
byte pData;

#include "F:\Arduino\Ook_OSV4\record.h"


//#include "decodeOTIO.h"
//DecodeOTIO Otio;

Record rec ;


void ext_int_1(void) {
    // determine the pulse length in microseconds, for either polarity
    pData=digitalRead(PDATA);
    
    pulse = micros() - last;
//    last += pulse;
    last = micros() ;
}


void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(10);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  char buff[50];
  sprintf(buff, "\nListening at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);
  pinMode(PDATA, INPUT);
  pinMode(LED, OUTPUT);
  Pin     = 0 ;
  pulse = 0 ;
  NbPulse = 0  ;
  last=0;
  attachInterrupt(1, ext_int_1, CHANGE);
 
  radio.setMode(RF69_MODE_RX);
  np=0;
  LastReceive=0;
  nb=0;
  nbc=0;
  T=0;
  rec.init();
}

#define HEXTODEC(AH) AH = AH-'0'; if (AH>9) AH = AH -( 7 );if (AH>15) AH = AH - 0x20 ;


void loop() {

    cli();
    unsigned long p = pulse/100;
    pulse = 0;
    sei();
    /* pulse > 2ms */
    byte data = pData ;

    if ( (p>5) ){
//      rec.AddFifo( p );
       rec.printDelta(p,300);
    }
    if ( (p>70)&& (data==1) ){
//        rec.PrintFifo(54);
    }

  //process any serial input
  if (Serial.available() > 0)
  {
    char input = Serial.read();
    if (input == 'p') { //print pulse 
      rec.Print();
      np=0;

    }
  
    
  }

  if (digitalRead(PDATA)==1)
    digitalWrite(LED,HIGH);
  else
     digitalWrite(LED,LOW);
  
}

