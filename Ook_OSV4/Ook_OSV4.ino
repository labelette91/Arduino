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
HomeEasyTransmitter easy(PDATA,PCLK,ledPin);

#ifndef DOMOTIC
#include  "reportSerialAscii.h"
#endif
#include "domotic.h"

void reportSerial (const char* s, class DecodeOOK& decoder) 
{
#ifndef DOMOTIC
            reportSerialAscii("OSV2", decoder.getData(),decoder.pos);  
#else            
//            reportDomotic( decoder);  
            reportDomoticTempHum ( decoder.getData());
#endif      
}

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
    DomoticInit();
    PulseLed();
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

void loop () {
    cli();
    word p = pulse;
    pulse = 0;
    sei();
    

#ifndef DOMOTIC
//print RSSI
/*
if ((millis() - Last ) > 5000 ) {
      Serial.print(radio.readRSSI());  
      Last = millis() ;
    }
*/    
#endif     

     Dt = (millis() - LastReceive)/1000;
     if (Dt==5)
         digitalWrite(ledPin, LOW);  

    if (Dt==1){
      // -1 = user cal factor, adjust for correct ambient
/*       byte temperature =  radio.readTemperature(-1); 
#ifndef DOMOTIC
       Serial.print  ("T:");
       Serial.println(temperature,DEC);
#else
       reportDomotic ( temperature , 0x45  , 0  );
#endif
       LastReceive += 1000;
       */
    }    
    if (p != 0) {
        NbPulse++;
        if (orscV2.nextPulse(p))
        {
          //data valide checksum
          if (checksum2(orscV2.getData()) )
          {
//            Dt = (millis() - LastReceive)/1000;
            if (Dt!=0){
//              PulseLed();
              digitalWrite(ledPin, HIGH);  
              reportSerial("OSV2", orscV2);  
            }
            LastReceive = millis() ;
    	    NbReceive ++ ;     
            NbPulse=0 ;
          }
          else
          {
#ifndef DOMOTIC
           Serial.println("Bad checksum");
#endif     
          }
          orscV2.resetDecoder();		
        }
        if (Otio.nextPulse(p,pinData )) 
        #ifndef DOMOTIC
           Otio.ReportSerial();    
        #else
           reportDomotic ( Otio.getTemperature() , 0 , Otio.getId() );
        #endif

    }

//check domotic send command reception
  if (DomoticReceive()){
//if (Serial.available()){
//    byte b = Serial.read();
  	
    detachInterrupt(1);
    easy.initPin();
    radio.setMode(RF69_MODE_TX);

//    easy.setSwitch(true,55,1);    // turn on device 0
//    delay(3000);        // wait 3 sec
//    easy.setSwitch(false,55,1);    // turn on device 0
    //attente une secone max pour emetre si emission en cours
    if (radio.WaitCanSend(-70))
//    if ( radio.canSend(-80))
//      easy.setSwitch(Cmd.LIGHTING2.cmnd,Cmd.LIGHTING2.id4,Cmd.LIGHTING2.unitcode);    // turn on device 0
      easy.setSwitch(Cmd.LIGHTING2.cmnd,getLightingId (),Cmd.LIGHTING2.unitcode);    // turn on device 0

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

