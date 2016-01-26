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

#define NBPULSE 512
word Pulse[NBPULSE];

byte          Pin ;
byte          lastPin ;
unsigned long LastPulseTime ;
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
  lastPin = 0 ;
  LastPulseTime = 0 ;
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
}

#define HEXTODEC(AH) AH = AH-'0'; if (AH>9) AH = AH -( 7 );if (AH>15) AH = AH - 0x20 ;


#include "decodeOTIO.h"

DecodeOTIO Otio(3);

void print4 (long p){
if (p<10)Serial.print("000");
else if (p<100)Serial.print("00");
else if (p<1000)Serial.print("0");
Serial.print(p);
Serial.print(' ');
};

void print (long p){
if (p<10)Serial.print("0");
Serial.print(p);
Serial.print(' ');
};



void Dump(){
      Serial.print(NbPulse);
      Serial.print(' ');
      for (word i=0;i<NbPulse;i++){
//        if (i%32==0)
        byte b = Otio.getPulseBit(Pulse[i]);
//        Serial.print(b);
        Serial.print(Pulse[i]/100 )    ;        
        Serial.print(' ');
//      if ( Pulse[i] > 6000)
//          Serial.println();
      }
        NbPulse=0;
        np=0;
//        Serial.print(T,BIN);
        Serial.print(T,HEX);
        Serial.println();
}


void DumpTemps(){
      Serial.print(NbPulse);
      Serial.print(' ');
      for (word i=0;i<NbPulse;i++){
        if (i%32==0)
          Serial.println();
        print(Pulse[i]/100 )    ;
        Serial.print(' ');
      }
      NbPulse=0;
      np=0;
      Serial.println();
}


void dump(){
      Dt = (millis() - LastReceive)/1000;
/*      if (nb==1) {
        Dump();
        nb++;
      }
      if  (nb==2){
        Dump();
        nb=0;
      }
*/       
//     if (Dt >2){
        Dump();
//        nb++;
//      }
  NbPulse=0;
      LastReceive = millis() ;
}


void pr(unsigned long p){
   Serial.print(p);
   if ((np++%16)==0)
      Serial.println(' ');
   else   
     Serial.print(' ');
    
}




void loop() {

    cli();
    unsigned long p = pulse;
    pulse = 0;
    sei();
    /* pulse > 2ms */
//    byte data = digitalRead(PDATA) ;
    byte data = pData ;
    if ( (p>1000)&& (data==1) ){
      np++;
    }
    if ( (p>1000)&&(np>0) ){
      if (NbPulse<NBPULSE)  Pulse[NbPulse++]=p;
      
      if ( p > 6000){
        dump();
        T=0;
      } 
      else if( p > 1000)
      {
        T = T << 1;
        T = T | Otio.getPulseBit(p);
      }   
      if (NbPulse>=50){
//        dump();
        T=0;
        NbPulse=0;
        np=0;        
      }
      
  }
    if (Otio.nextPulse(p,data )) {
      Otio.ReportSerial();    
  }

/*    if (p>4000) { 
      pr(p);        
*/  

  //process any serial input
  if (Serial.available() > 0)
  {
    char input = Serial.read();
    if (input == 'p') { //print pulse 
      DumpTemps()  ;
    }
    
    Serial.print(input); 
    nbc++;
    if (nbc==4){
        Serial.println();
      nbc=0;
    }

  }

  if (digitalRead(PDATA)==1)
    digitalWrite(LED,HIGH);
  else
     digitalWrite(LED,LOW);
  
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
