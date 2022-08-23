#ifdef WIN32
#include "vspde.h"
#endif
//si = define  : report serial forma domoticz (binaire)
//si =  : report serial format text 

//#define DOMOTIC 1
//#define RFM69_ENABLE

#define OTIO_ENABLE 1
#define OOK_ENABLE  1
//#define HAGER_ENABLE 1
//#define HOMEEASY_ENABLE 1
//#define MD230_ENABLE 1
//#define RUBICSON_ENABLE 1
#define  HIDEKI_ENABLE        
//#define  BMP180_ENABLE        

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

#ifdef HOMEEASY_ENABLE
#include "DecodeHomeEasy.h"
DecodeHomeEasy HEasy ;
#endif

#ifdef MD230_ENABLE
#include "DecodeMD230.h"
DecodeMD230 MD230(2) ;
#endif

#ifdef OTIO_ENABLE        
#include <DecodeOTIO.h>
DecodeOTIO Otio(3);  
#endif

#include "Fifo.h"
TFifo  fifo;

#define PORT 2
//#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
// #if defined(__AVR_ATmega2560__) 
// #define ledPin  13 sur UNO
// #define ledPin  9 sur anarduino
// #define ledPin  2 sur 8266

#define ledPin  LED_BUILTIN

#ifdef ESP8266
#define PDATA 5 // GPIO5 = D1 sur la carte wiimos
#define PCLK  4 // GPIO4 = D2 sur la carte wiimos

#else
#define PDATA 3 //pin for data input/output
#define PCLK  4 //pin for clk  input/output
#endif

//last packet data received ident
byte data0,data1,data2,data3;

word 		NbReceive;
word 	NbPulse  ;
word 	NbPulsePerSec ;

//etat du pulse
byte        PulsePinData;
word 		Seconds;
word 		lastSeconds;
word 		lastMinute ;

//le signal du RFM69 entre sur int ext    d3 : int1
//sur 8266 : interrupt shall be in IRAM
#ifdef ESP8266
void ICACHE_RAM_ATTR ext_int_1(void) {
#else
void ext_int_1(void) {
#endif

    static unsigned long  last;
    byte            pinData;
    word pulse;

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

#ifdef RFM69_ENABLE
#include <RFM69.h>
#include <RFM69registers.h>
#include <SPI.h>
RFM69 radio;
#endif

// This is the Home Easy controller
// This example will use a 433AM transmitter on
// pin 3 : data pin
// pin 4 : clk  pin
HomeEasyTransmitter easy(PDATA,PCLK,ledPin);

#include  "reportSerial.h"

#ifndef DOMOTIC
#include  "reportSerialAscii.h"
#endif
#include "domotic.h"

#include "hager.h"

#ifdef HIDEKI_ENABLE        
#include "tfaDecoder.h"
Hideki tfa3208;
#endif

#ifdef BMP180_ENABLE
#include <Wire.h>
#include <BMP180advanced.h>

/*
BMP180advanced(resolution)

resolution:
BMP180_ULTRALOWPOWER - pressure oversampled 1 time  & power consumption 3uA
BMP180_STANDARD      - pressure oversampled 2 times & power consumption 5uA
BMP180_HIGHRES       - pressure oversampled 4 times & power consumption 7uA
BMP180_ULTRAHIGHRES  - pressure oversampled 8 times & power consumption 12uA, library default
*/
BMP180advanced myBMP(BMP180_ULTRAHIGHRES);

float coefPressureSeaLevel;

#endif


inline static void write(word w)
{
  static byte nbc = 0;
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
  nbc++;
  if ((nbc%16) == 0 )
    Serial.write('\n');
  else
    Serial.write(' ');

  
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
	  NbReceive   = 0;

#ifndef DOMOTIC
    Serial.begin(2000000);
    //Serial.begin(115200);
#else
    Serial.begin(38400);
#endif    
   // initialize the LED pin as an output:
    pinMode(ledPin, OUTPUT);       
    pinMode(PDATA, INPUT);
   
    attachInterrupt(digitalPinToInterrupt(PDATA) , ext_int_1, CHANGE);

#ifdef RFM69_ENABLE
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

#ifdef HOMEEASY_ENABLE
    HEasy.resetDecoder();
#endif
#ifdef MD230_ENABLE
	  MD230.resetDecoder();
#endif
		DomoticInit();

#ifdef BMP180_ENABLE
        bool status = myBMP.begin();
        //compute coeficien niveau de la mer     pressureSeaLevel =  (pressure / pow(1.0 - (float)trueAltitude / 44330, 5.255));
        // coefPressureSeaLevel = 0,991730   1/coefPressureSeaLevel = 1.008338963225878
        float trueAltitude = 70.0;
        coefPressureSeaLevel =  pow(1.0 - (float)trueAltitude / 44330, 5.255) ;

        #ifndef DOMOTIC
        if (status != true)
        {
            Serial.println(F("Bosch BMP180/BMP085 is not connected or fail to read calibration coefficients"));
        }
  
        Serial.println(F("Bosch BMP180/BMP085 sensor is OK ")); //(F()) saves string to flash & keeps dynamic memory free
        Serial.print(" coefPressureSeaLevel ");
        Serial.println(coefPressureSeaLevel,6 );
        #endif

#endif

delay(100);
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

//1 = dump pulse len to serial
byte dumpPulse=0;

void loop () {

    word rssi;
    word p = fifo.get();

		if (p > 0 ) {
            if (dumpPulse)
                if (p>200){		
                        write(p);
                }
            //get pinData
			PulsePinData = p & 1;
			NbPulse++;
			Seconds = millis() / 1000;
            //every seconds
			if (Seconds != lastSeconds)
			{
					lastSeconds = Seconds ;
					NbPulsePerSec = NbPulse;
					NbPulse = 0;
//          Serial.print(".");
			}
            //every minute
            if ((Seconds/60)!= lastMinute )
            {
                if (!DomoticReceptionInProgress())
                {
                    lastMinute = Seconds/60;
#ifdef BMP180_ENABLE
                    float temp = myBMP.getTemperature() ;
                    uint32_t pressureInPa = myBMP.getPressure() ;
#ifndef DOMOTIC
                    Serial.print(F("Temperature.......: ")); Serial.print(temp, 1);              Serial.println(F(" +-1.0C"));
                    Serial.print(F("Pressure..........: ")); Serial.print(pressureInPa);         Serial.println(F(" +-1hPa"));
                    Serial.print(F("Pressure Sea Level: ")); Serial.print((float)pressureInPa/coefPressureSeaLevel);         Serial.println(F(" +-1hPa"));
#else                    
                    reportDomoticTempBaro (1 , temp , pressureInPa/100  , 50.0  , 1 );

                    reportDomoticTempHumBaro (1,1  , temp , pressureInPa/100 ,  1,  0  , 0xff , 0xFF   );
#endif
#endif
                }
                lastMinute = Seconds/60;
            }


			if (orscV2.nextPulse(p))
			{
					// -1 : on retire le byte de postambule
//				if (checksum(orscV2.getData(), orscV2.pos - 1))
                if (orscV2.isValid())
				{
                    //if ( orscV2.data[0] == CMR180_ID0 ) dumpPulse=0;
                    //if ( orscV2.data[0] == 0x1A       ) dumpPulse=0;  //THGR228N

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
					Serial.print("Bad checksum ");

                    Serial.print(orscV2.total_bits); Serial.print(' ');
                    Serial.print(orscV2.state     ); Serial.print(' ');

					reportSerial("OSV2", orscV2);
#endif     
				}
				orscV2.resetDecoder();
			}
#ifdef OTIO_ENABLE        
			if (Otio.nextPulse(p, PulsePinData)) {
                dumpPulse=0;
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
					  reportDomoticTempHum (Rubicson.getTemperature(), Rubicson.gethumidity(), Rubicson.getId(), Rubicson.getChannel(), Rubicson.getBatteryLevel(),sTypeTH10_RUBiCSON);

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
#ifdef RFM69_ENABLE
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

#ifdef HOMEEASY_ENABLE
//		  && (HEasy.total_bits == 0)
#endif
#ifdef MD230_ENABLE
//		  && (MD230.total_bits == 0)
#endif
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
	    detachInterrupt(digitalPinToInterrupt(PDATA));

	    easy.initPin();
#ifdef RFM69_ENABLE
	    radio.setMode(RF69_MODE_TX);
#endif
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
        attachInterrupt(digitalPinToInterrupt(PDATA) , ext_int_1, CHANGE);
#ifdef RFM69_ENABLE
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
    static long int LastPrintRSSI ;
//print RSSI
if ((millis() - LastPrintRSSI ) > 5000 ) {
#ifdef RFM69_ENABLE
      Serial.print(radio.readRSSI());  
#endif
      LastPrintRSSI = millis() ;
}
}

void reportTemperatureToDomotic()
{
#ifdef RFM69_ENABLE
      // -1 = user cal factor, adjust for correct ambient
      byte temperature =  radio.readTemperature(-1); 

#ifndef DOMOTIC
       Serial.print  ("T:");
       Serial.println(temperature,DEC);
#else
       reportDomoticTemp ( temperature , 0x45  , 0 ,15 );
#endif
#endif
}
