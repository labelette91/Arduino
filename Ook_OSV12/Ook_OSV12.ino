//si = define  : report serial forma domoticz (binaire)
//si =  : report serial format text 

//#define DOMOTIC 1

#define OTIO_ENABLE 1
#define OOK_ENABLE  1
//#define HAGER_ENABLE 1
//#define HOMEEASY_ENABLE 1
//#define MD230_ENABLE 1
//#define RUBICSON_ENABLE 1
#define  HIDEKI_ENABLE        

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
#include "rubicson.h"

OregonDecoderV2 orscV2;
HagerDecoder    hager;
DecodeRubicson  Rubicson;

/* OregonDecoderV3 orscV3; */

//end oregon 

#include "DecodeHomeEasy.h"
DecodeHomeEasy HEasy ;

#include "DecodeMD230.h"
DecodeMD230 MD230(2) ;


#ifdef OTIO_ENABLE        
#include <DecodeOTIO.h>
DecodeOTIO Otio(3);  
#endif

#include "Fifo.h"
TFifo  fifo;

#define PORT 2
//#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
#if defined(__AVR_ATmega2560__) 
#define ledPin  13
#else
#define ledPin  9
#endif

#define PDATA 3 //pin for data input/output
#define PCLK  4 //pin for clk  input/output

volatile word pulse;

word  	LastReceive ;

//last packet data received ident
byte data0,data1,data2,data3;

word 		NbReceive;
word 		Dt;
word 	NbPulse  ;
word 	NbPulsePerSec ;
byte            pinData;
//etat du pulse
byte            PulsePinData;

//le signal du RFM69 entre sur int ext    d3 : int1

void ext_int_1(void) {
    static unsigned long  last;

    // determine the pulse length in microseconds, for either polarity
    pulse = micros() - last;
    last += pulse;

		pinData = digitalRead(PDATA);
		
		//calcul etat du pulse que l'on mesure
		if (pinData == 1)
			//tranistion 0--1 : etat pulse = 0 : bit 0 = 0
			pulse &=0xFFFE ;
		else
			//tranistion 1--0 : etat pulse = 1 : bit 0 = 1
			pulse |= 1 ;

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

#ifdef HIDEKI_ENABLE        
#include "c:\arduino\tfa\tfaDecoder.h"
Hideki tfa3208;
#endif


inline static void write(word w)
{
  if (w>=1000)
  {
  Serial.write(w/1000+'0'); w = w % 1000; 
  Serial.write(w/100 +'0'); w = w % 100;  
  Serial.write(w/10  +'0') ; w = w % 10;  
  Serial.write(w     +'0') ;              
  }
  else if (w>=100)
  {
  Serial.write(w/100 +'0'); w = w % 100;  
  Serial.write(w/10  +'0') ; w = w % 10;  
  Serial.write(w     +'0') ;              
  }
  else if (w>=10)
  {
  Serial.write(w/10  +'0') ; w = w % 10;  
  Serial.write(w     +'0') ;              
  }
  else 
  {
  Serial.write(w     +'0') ;              
  }
  Serial.write('\n');
  
}

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

#ifndef DOMOTIC
    Serial.begin(2000000);
#else
    Serial.begin(38400);
#endif    
   // initialize the LED pin as an output:
    pinMode(ledPin, OUTPUT);       
    pinMode(PDATA, INPUT);
   
    pulse=0;

    attachInterrupt(1, ext_int_1, CHANGE);
#ifdef RFM69
    radio.initialize(RF69_433MHZ,1,100);

    radio.setMode(RF69_MODE_RX);

		    PulseLed();
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
#endif     

    HEasy.resetDecoder();
	  MD230.resetDecoder();

		DomoticInit();

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
	word rssi;
    word p = fifo.get();

		if (p != 0) {
//            if (p>200)		write(p/10);

            //get pinData
			PulsePinData = p & 1;
			NbPulse++;
			Dt = millis() / 1000;
			if (Dt != LastReceive)
			{
					LastReceive = Dt ;
					NbPulsePerSec = NbPulse;
					NbPulse = 0;
//          Serial.print(".");
			}
			if (orscV2.nextPulse(p))
			{
					// -1 : on retire le byte de postambule
				if (checksum(orscV2.getData(), orscV2.pos - 1))
				{
					Dt = (millis() - LastReceive) / 1000;
					if ((data3 != orscV2.data[3]) || (data0 != orscV2.data[0]) || (data1 != orscV2.data[1]) || (data2 != orscV2.data[2])) {
						PulseLed();
#ifdef OOK_ENABLE        
						reportSerial("OSV2", orscV2);
#endif
						data0 = orscV2.data[0];
						data1 = orscV2.data[1];
						data2 = orscV2.data[2];
						data3 = orscV2.data[3];

					}
					NbReceive++;
					if (NbReceive > 25)
					{
						resetLastSensorValue();
						NbReceive = 0;
					}

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
			if (Otio.nextPulse(p, pinData)) {
#ifndef DOMOTIC
				Otio.ReportSerial();
#else
				reportDomoticTemp(Otio.getTemperature(), Otio.getId(), 0, Otio.getBatteryLevel());
#endif
				PulseLed();
			}
#endif      	

#ifdef HOMEEASY_ENABLE
			if (HEasy.nextPulse(p, pinData)) {
#ifndef DOMOTIC
				HEasy.ReportSerial();
#else
				reportDomoticHomeEasy(HEasy.getData(), HEasy.getBytes());
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
			if (pinData == 1) p -= 100; else p += 100;

			if (hager.nextPulse(p)) {
#ifndef DOMOTIC
				hager.reportSerial();
#else
				reportHagerDomotic(hager.getData(), hager.pos);
#endif
				hager.resetDecoder();
				PulseLed();
			}
#endif        


#ifdef RUBICSON_ENABLE        
			if (Rubicson.nextPulse(p, PulsePinData)) {

				if (Rubicson.newPacket() ) {

#ifndef DOMOTIC
					if (Rubicson.isOtio())
						Rubicson.ReportSerialOtio();
					else
						Rubicson.ReportSerial();
#else
					if (Rubicson.isOtio())
						reportDomoticTemp(Rubicson.getTemperatureOtio(), Rubicson.getIdOtio(), 0           , Rubicson.getBatteryLevelOtio());
					else
//  					reportDomoticTemp(Rubicson.getTemperature(), Rubicson.getId(), Rubicson.getChannel(), Rubicson.getBatteryLevel());
					  reportDomoticTempHum (Rubicson.getTemperature(), Rubicson.gethumidity(), Rubicson.getId(), Rubicson.getChannel(), Rubicson.getBatteryLevel());

#endif
					PulseLed();
				}
				Rubicson.resetDecoder();
			}
#endif      	

#ifdef HIDEKI_ENABLE        
      managedHideki(&tfa3208,p);
#endif

    }

	//read serial input & fill receive buffe(
	DomoticReceive();

	//check domotic send command reception
	//attente une secone max pour emetre si emission en cours -80--> -70
	//pas de reception en cours
	if (   (DomoticPacketReceived)
#ifdef RFM69
		  && (radio.canSend(-70)   )
#endif
#ifdef OTIO_ENABLE        
		  && (Otio.total_bits ==0)
#endif
#ifdef OOK_ENABLE        
		  && (orscV2.total_bits == 0)
#endif
#ifdef RUBICSON_ENABLE        
		  && (Rubicson.total_bits == 0)
#endif

//		  && (HEasy.total_bits == 0)
//		  && (MD230.total_bits == 0)
		)
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
#ifdef RFM69
	    detachInterrupt(1);
	    easy.initPin();
	    radio.setMode(RF69_MODE_TX);
			delay(10);
	    	
	    if (Cmd.LIGHTING2.packettype==pTypeLighting2) 
			{  //
				if (Cmd.LIGHTING2.subtype == sTypeHEU) 	         //if home easy protocol : subtype==1
				{
					easy.setSwitch(Cmd.LIGHTING2.cmnd, getLightingId(), Cmd.LIGHTING2.unitcode);    // turn on device 0
					Cmd.LIGHTING2.subtype = 1;
				}
				else if (Cmd.LIGHTING2.subtype == sTypeAC) 	         //if hager protocol : subtype==0
				{
					ManageHager(Cmd.LIGHTING2.id4, Cmd.LIGHTING2.unitcode, Cmd.LIGHTING2.cmnd);
					Cmd.LIGHTING2.subtype = 0;
				}
				else
					Cmd.LIGHTING2.subtype = 2;

	    }
			else
				Cmd.LIGHTING2.subtype = 3;

			//acknoledge 
			Cmd.LIGHTING2.packettype = pTypeUndecoded;
			Cmd.LIGHTING2.packetlength = 7;
			Cmd.LIGHTING2.id1 = rssi >> 8;
			Cmd.LIGHTING2.id2 = rssi & 0x00ff ;
			Cmd.LIGHTING2.id3 = NbPulsePerSec >> 8;
			Cmd.LIGHTING2.id4 = NbPulsePerSec & 0x00ff;

			

			Serial.write((byte*)&Cmd.LIGHTING2, Cmd.LIGHTING2.packetlength + 1);

	
	    pinMode(PDATA, INPUT);
	    attachInterrupt(1, ext_int_1, CHANGE);
	    radio.setMode(RF69_MODE_RX);
#endif
    }
  	digitalWrite(ledPin,LOW);
		DomoticPacketReceived = false;
    
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
       reportDomoticTemp ( temperature , 0x45  , 0 ,15 );
#endif

}
