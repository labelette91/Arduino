#include <RFM69.h>
#include <SPI.h>
// Oregon V2 decoder added - Dominique Pierre
// Oregon V3 decoder revisited - Dominique Pierre
// New code to decode OOK signals from weather sensors, etc.
// 2010-04-11 <jcw@equi4.com> http://opensource.org/licenses/mit-license.php
// $Id: ookDecoder.pde 5331 2010-04-17 10:45:17Z jcw $

#include "OOKDecoder.h"
#include <HomeEasyTransmitter.h>

OregonDecoderV2 orscV2;
/* OregonDecoderV3 orscV3; */

//end oregon 

#include "C:\Users\jeux\Documents\Arduino\otio\decodeOTIO.h"
DecodeOTIO Otio(3);

#define PORT 2
#define ledPin  9
#define PDATA 3 //pin for data input/output
#define PCLK  4 //pin for clk  input/output

volatile word pulse;

long int 	LastReceive ;
int 		NbReceive;
word 		Dt;
long int 	NbPulse  ;
byte            pinData;

//le signal du RFM69 entre sur int ext    d3 : int1

#define DOMOTIC 1

void ext_int_1(void) {
    static unsigned long  last;
    pinData=digitalRead(PDATA);

    // determine the pulse length in microseconds, for either polarity
    pulse = micros() - last;
    last += pulse;
//    last = micros() ;
    
}
#include "Oregon.h"

RFM69 radio;

// This is the Home Easy controller
// This example will use a 433AM transmitter on
// pin 3 : data pin
// pin 4 : clk  pin

#ifndef DOMOTIC
#include  "reportSerialAscii.h"
#endif
#include "domotic.h"

#include "hager.h"


void setup () {
	  LastReceive = 0 ;
	  NbReceive   = 0;
    
    Serial.begin(38400);
//    Serial.begin(115200);
    
   // initialize the LED pin as an output:
    pinMode(ledPin, OUTPUT);       
    pinMode(PDATA, INPUT);
   
    pulse=0;

    radio.initialize(RF69_433MHZ,1,100);
    attachInterrupt(1, ext_int_1, CHANGE);

    radio.setMode(RF69_MODE_RX);
    PulseLed();
    Serial.print("run");  
}

void PulseLed()
{
      if (digitalRead(ledPin)== LOW) 
         digitalWrite(ledPin, HIGH);  
      else
         digitalWrite(ledPin, LOW);  
}
#define HEXTODEC(AH) AH = AH-'0'; if (AH>9) AH = AH -( 7 );if (AH>15) AH = AH - 0x20 ;
  long int 	Last ;

void 	HagerPrint();
 
void loop () {
    cli();
    word p = pulse;
    pulse = 0;
    sei();
    
//check domotic send command reception
if (Serial.available()){
    byte b = Serial.read();
    while (!Serial.available()) ;

    byte c = Serial.read();
  	
    detachInterrupt(1);
    radio.setMode(RF69_MODE_TX);
    pinMode(PDATA, OUTPUT);

    {
      b = b-'0';
      if (b>9) b = b - 7;
      Serial.print (b);  
      c = c-'0';
      Serial.println(c);  

//      for (byte i=0;i<255;i++){
        ManageHager(b,c);
				HagerPrint();
        delay(2000);
//      }
    }

    pinMode(PDATA, INPUT);
    attachInterrupt(1, ext_int_1, CHANGE);
    radio.setMode(RF69_MODE_RX);
  }

}
void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}

