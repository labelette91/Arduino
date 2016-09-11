#define DOMOTIC 1
#define OTIO_ENABLE 1
#define OOK_ENABLE  1
#define HAGER_ENABLE 1
//#define HOMEEASY_ENABLE 1
#define MD230_ENABLE 1

#include <RFM69.h>
#include <RFM69registers.h>
#include <SPI.h>
// Oregon V2 decoder added - Dominique Pierre
// Oregon V3 decoder revisited - Dominique Pierre
// New code to decode OOK signals from weather sensors, etc.
// 2010-04-11 <jcw@equi4.com> http://opensource.org/licenses/mit-license.php
// $Id: ookDecoder.pde 5331 2010-04-17 10:45:17Z jcw $

#include "OOKDecoder.h"
#include <HagerDecoder.h>
#include <HomeEasyTransmitter.h>

OregonDecoderV2 orscV2;
HagerDecoder    hager;
/* OregonDecoderV3 orscV3; */

//end oregon 

#include "DecodeHomeEasy.h"
DecodeHomeEasy HEasy ;

#include "DecodeMD230.h"
DecodeMD230 MD230(4) ;


//#include "C:\Users\jeux\Documents\Arduino\otio\decodeOTIO.h"
//#include "C:\Documents and Settings\F206150\Mes documents\Arduino\otio\decodeOTIO.h"
#include <DecodeOTIO.h>
DecodeOTIO Otio(3);  

#include "Fifo.h"
TFifo  fifo;

#define PORT 2
#define ledPin  9
#define PDATA 3 //pin for data input/output
#define PCLK  4 //pin for clk  input/output

volatile word pulse;

long int 	LastReceive ;

//last packet data received ident
byte data0,data1,data2;

int 		NbReceive;
word 		Dt;
long int 	NbPulse  ;
byte            pinData;

//le signal du RFM69 entre sur int ext    d3 : int1

void ext_int_1(void) {
    static unsigned long  last;
    pinData=digitalRead(PDATA);

    // determine the pulse length in microseconds, for either polarity
    pulse = micros() - last;
    last += pulse;
//    last = micros() ;
    fifo.put(pulse);
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

#include "hager.h"

void reportSerial (const char* s, class DecodeOOK& decoder) 
{
#ifndef DOMOTIC
            reportSerialAscii("OSV2", decoder.getData(),decoder.pos);  
#else            
            reportDomotic ( decoder.getData(),decoder.pos);
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
//    PulseLed();
    //2400 bauds bit rate 3,4
    radio.writeReg(REG_BITRATEMSB,RF_BITRATEMSB_2400);
    radio.writeReg(REG_BITRATELSB,RF_BITRATELSB_2400);
    // RegOokPeak : 1b
//    radio.writeReg(REG_OOKPEAK,RF_OOKPEAK_THRESHTYPE_FIXED+RF_OOKPEAK_PEAKTHRESHDEC_111);
    radio.writeReg(REG_OOKPEAK,RF_OOKPEAK_THRESHTYPE_PEAK +RF_OOKPEAK_PEAKTHRESHDEC_000);

    // REG_OOKFIX  88 au lieu de 70
//    radio.writeReg(REG_OOKFIX,84 );
    //lna 50 h    
    radio.writeReg(REG_LNA, RF_LNA_ZIN_50);
    HEasy.resetDecoder();
	  MD230.resetDecoder();
#ifndef DOMOTIC
    Serial.print("Version ");
    Serial.println(VERSION);
#endif     
   
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
/*    cli();
    word p = pulse;
    pulse = 0;
    sei();
*/
    word p = fifo.get();

/*     Dt = (millis() - LastReceive)/1000;
     if (Dt==5)
         digitalWrite(ledPin, LOW);  

    if (Dt==1){
    }    */
    if (p != 0) {
        NbPulse++;
        if (orscV2.nextPulse(p))
        {
          //data valide checksum
//          if (checksum2(orscV2.getData()) ) // test checksul pour len = 10
					// -1 : on retire le byte de postambule
					if (checksum(orscV2.getData(),orscV2.pos-1) )
          {
            Dt = (millis() - LastReceive)/1000;
            if ( (Dt!=0) || (data0!=orscV2.data[0]) || ( data1!=orscV2.data[1]) || (data2!=orscV2.data[2] ) ) {
              PulseLed();
//              digitalWrite(ledPin, HIGH);  
#ifdef OOK_ENABLE        
              reportSerial("OSV2", orscV2);  
#endif
							data0=orscV2.data[0];
							data1=orscV2.data[1];
							data2=orscV2.data[2];

            }
            LastReceive = millis() ;
    	    NbReceive ++ ;     
    	    	if (NbReceive>25 ) 
    	    	{ 
    	    		resetLastSensorValue();
    	    		NbReceive=0;
    	    	}

            NbPulse=0 ;
          }
          else
          {
#ifndef DOMOTIC
           Serial.println("Bad checksum");
           reportSerial("OSV2", orscV2);  
#endif     
          }
          orscV2.resetDecoder();		
        }
#ifdef OTIO_ENABLE        
        if (Otio.nextPulse(p,pinData )) {
        #ifndef DOMOTIC
           Otio.ReportSerial();    
        #else
           reportDomoticTemp ( Otio.getTemperature() , Otio.getId()  , 0  );
        #endif
        PulseLed();
      	}
#endif      	

#ifdef HOMEEASY_ENABLE
        if (HEasy.nextPulse(p,pinData )) {
        #ifndef DOMOTIC
           HEasy.ReportSerial();    
        #else
           reportDomoticHomeEasy ( HEasy.getData() , HEasy.getBytes ()  );
        #endif
        PulseLed();
      	}
#endif

#ifdef MD230_ENABLE
		if (MD230.nextPulse(p, pinData)) {
#ifndef DOMOTIC
			MD230.ReportSerial();
#else
			reportDomoticMD230(MD230.getData(), MD230.getBytes());
#endif
			PulseLed();
		}
#endif

#ifdef HAGER_ENABLE        
        //si pulse bas on enleve 100micros sinon on ajoute 100micros
        //pour compenser etat haut tronquer
        if (pinData==1) p -=100; else p +=100 ;

        if (hager.nextPulse(p)) {
          #ifndef DOMOTIC
             hager.reportSerial();    
          #else
             reportHagerDomotic ( hager.getData (),hager.pos );
          #endif
          hager.resetDecoder();   
          PulseLed();
        }
#endif        


    }

//check domotic send command reception
  if (DomoticReceive())
  {
  	digitalWrite(ledPin,HIGH);
    //start receive cmd
    if ( (Cmd.ICMND.packettype == 0)&& (Cmd.ICMND.cmnd==cmdStartRec) ) {  
    	DomoticStartReceive();
    }
    else if ( (Cmd.ICMND.packettype == 0)&& (Cmd.ICMND.cmnd==cmdSTATUS) ) {  
    	DomoticStatus();
    }
    else
    {
	    detachInterrupt(1);
	    easy.initPin();
	    radio.setMode(RF69_MODE_TX);
	    //attente une secone max pour emetre si emission en cours -80--> -70
	    radio.WaitCanSend(-70);
	    {
	    	
	      if (Cmd.LIGHTING2.packettype==pTypeLighting2) {  //
		      if (Cmd.LIGHTING2.subtype==sTypeHEU) 	         //if home easy protocol : subtype==1
		        easy.setSwitch(Cmd.LIGHTING2.cmnd,getLightingId (),Cmd.LIGHTING2.unitcode);    // turn on device 0

		      else if (Cmd.LIGHTING2.subtype==sTypeAC) 	         //if hager protocol : subtype==0
		        ManageHager();
          //acknoledge 
					// Cmd.LIGHTING2.subtype==sTypeKambrook;
					// Serial.write((byte*)&Cmd.LIGHTING2,Cmd.LIGHTING2.packetlength+1  );
	    	}
	    }
	
	    pinMode(PDATA, INPUT);
	    attachInterrupt(1, ext_int_1, CHANGE);
	    radio.setMode(RF69_MODE_RX);
    }
  	digitalWrite(ledPin,LOW);
    
  }

}
void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}

void printRSSI()
{
//print RSSI
if ((millis() - Last ) > 5000 ) {
      Serial.print(radio.readRSSI());  
      Last = millis() ;
}
}

void reportTemperatureToDomotic()
{
      // -1 = user cal factor, adjust for correct ambient
      byte temperature =  radio.readTemperature(-1); 

#ifndef DOMOTIC
       Serial.print  ("T:");
       Serial.println(temperature,DEC);
#else
       reportDomoticTemp ( temperature , 0x45  , 0  );
#endif

}

