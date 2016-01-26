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

unsigned long pulse ;
word          NbPulse  ;
byte          np;
static unsigned long  last;

unsigned long T ;
byte pData;

#include "C:\Users\jeux\Documents\Arduino\Ook_OSV4\record.h"
Record rec;


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
  Serial.println("Listening ");
  pinMode(PDATA, INPUT);
  pinMode(LED, OUTPUT);
  pulse = 0 ;
  NbPulse = 0  ;
  last=0;
  attachInterrupt(1, ext_int_1, CHANGE);
 
  radio.setMode(RF69_MODE_RX);
  np=0;
  T=0;
  rec.init();
}

#define HEXTODEC(AH) AH = AH-'0'; if (AH>9) AH = AH -( 7 );if (AH>15) AH = AH - 0x20 ;


#include "decodeOTIO.h"

DecodeOTIO Otio(3);

void Dump(){
      Serial.print(NbPulse);
      Serial.print(' ');
      Serial.print(T,HEX);
      Serial.print(' ');
//        byte b = Otio.getPulseBit(Pulse[i]);
        rec.PrintFifo(NbPulse+4);
//        rec.reset() ;// NbPulse=0;
//       rec.Print();
//        Serial.println();
}

byte getPulseBit(byte p)
    {
    if ( p < 25)
      return 0 ;
    else
    if ( p < 65)
      return 1 ;
    else 
      return 2 ;
    }
    
void loop() {

    cli();
    unsigned long p = pulse;
    pulse = 0;
    sei();
    /* pulse > 2ms */
    byte data = pData ;
    byte pb = p /100;
    if (pb>10)
        rec.AddFifo(pb);

    if ( (pb>20)&&(data==1)&&(np==0) ){
      np = 1 ;
    }
    if ( (pb>10)&&(np>0) ){
//      rec.AddFifo(p/100);
      NbPulse++;      
      if ( pb > 60){
        Dump();
        NbPulse=0;
        T=0;
        np=0;
      } 
      else if( pb > 10)
      {
        T = T << 1;
        T = T | getPulseBit(pb);
      }   
    }
    if (p)
      if (Otio.nextPulse(p,data )) 
         Otio.ReportSerial();    

  //process any serial input
  if (Serial.available() > 0)
  {
    char input = Serial.read();
    if (input == 'p') { 
//     rec.Print();
    }
  }

  if (digitalRead(PDATA)==1)
    digitalWrite(LED,HIGH);
  else
     digitalWrite(LED,LOW);
}

