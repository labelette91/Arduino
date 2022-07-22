/***********************************************
  Auteur :      Pascal Cambier
  Nom :         TFA & Cie, Module 3 
  Version :     Prod v1 décembre 2019
  Description : - Réception satellites TFA Dostmann 30.3208.02 basé sur https://github.com/robwlakes/ArduinoWeatherOS
                - Réemmission en I2C après contrôle validité réception
                    (les satellites émettant deux fois de suite +/- toutes les minutes,
                     il suffit de vérifier si les 2 transmissions sont égales.)
  Références :  https://github.com/robwlakes/ArduinoWeatherOS
                https://github.com/madsci1016/Arduino-EasyTransfer/tree/master/EasyTransferI2C
*/
 
#include <Wire.h>
 
// RTC
 
//==================================
const byte MxCnl = 8 ; // Nombre max de canaux / satellites
int iCanal ; //le canal du satellite
String Spaquet; //tous les bits dans une String
String Spaquet2; //tous les bits dans une String
float fTemp; // Température
int iHum; // humidité
byte Nr = 0; // 0 = 1ère mesure
 
 
struct ST_TFA { // Réception / Émission même schéma chez l'esclave (module 4)
  int Canal ;
  float Temp;
  byte Hum;
};
 
ST_TFA mydata ; // pour émission i2c
ST_TFA Premier ;
 
// Étalonnage (mesures dans un endroit confiné et différence par rapport à la moyenne)
const float EtalonT[MxCnl] = { -0.3, -0.3, 0.2, 0.2, 0.1, 0, 0.3, -0.4};
const int EtalonH[MxCnl] = { -2, 0, 2, -2, -4, -2, 2, 3};
 
 
//############### MANCHESTER ################
 
//Interface Definitions
//int RxPin           = 8;   //The number of signal from the Rx
int RxPin           = 3;   //The number of signal from the Rx
int ledPin          = 13;  //The number of the onboard LED pin
 
// Variables for Manchester Receiver Logic:
//word    sDelay     = 250;  //Small Delay about 1/4 of bit duration  try like 250 to 500
//220 semble donner de meilleurs résultats
word    sDelay     = 220;
word    lDelay     = 500;
byte    polarity   = 1;
byte    tempBit    = 1;
byte    discards   = 0;
boolean firstZero  = false;
boolean noErrors   = true;
 
byte    headerBits = 10;
byte    headerHits = 0;
 
byte    dataByte   = 0;
byte    nosBits    = 0;
byte    maxBytes   = 10; // Attention, 6 et pas 5
byte    nosBytes   = 0;
 
byte    nosRepeats = 0;
 
byte  manchester[20];
 
 
void setup() {
 
  Serial.begin(115200);
 
  Wire.begin();
  //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc.
//  ET.begin(details(mydata), &Wire);
 
  pinMode(RxPin, INPUT);
  pinMode(ledPin, OUTPUT);
  lDelay = 2 * sDelay;
  eraseManchester();
 
  attachInterrupt(1, ext_int_1, CHANGE);

 
  Serial.println(F("START Module 3 Prod v1 : TFA >=> I2C"));
    sei();
 
}
 
volatile word 	pulse;
byte            pinData;
word 			NbPulse  ;
//etat du pulse
byte            PulsePinData;

#include "Fifo.h"
TFifo  fifo;

#include "tfaDecoder.h"
Hideki tfa3208;
 
void ext_int_1(void) {
    static unsigned long  last;

    // determine the pulse length in microseconds, for either polarity
    pulse = micros() - last;
    last += pulse;

		pinData = digitalRead(RxPin);
		
		//calcul etat du pulse que l'on mesure
		if (pinData == 1)
			//tranistion 0--1 : etat pulse = 0 : bit 0 = 0
			pulse &=0xFFFE ;
		else
			//tranistion 1--0 : etat pulse = 1 : bit 0 = 1
			pulse |= 1 ;

		fifo.put(pulse);
}
 
void loop() {

	
  // Routine épurée basé sur https://github.com/robwlakes/ArduinoWeatherOS
  tempBit = polarity ^ 1;
  noErrors = true;
  firstZero = false;
  headerHits = 0;
  nosBits = 0;
  nosBytes = 0;
  digitalWrite(ledPin, 0);
  tempBit = polarity ^ 1;
  noErrors = true;
  firstZero = false;
  headerHits = 0;
  nosBits = 0;
  nosBytes = 0;
  digitalWrite(ledPin, 0);
  while (noErrors && (nosBytes < maxBytes)) {
    while (digitalRead(RxPin) != tempBit) {
    }
    delayMicroseconds(sDelay);
    digitalWrite(ledPin, 0);
    if (digitalRead(RxPin) != tempBit) {
      noErrors = false;
    }
    else {
      byte bitState = tempBit ^ polarity;
      delayMicroseconds(lDelay);
      if (digitalRead(RxPin) == tempBit) {
        tempBit = tempBit ^ 1;
      }
      if (bitState == 1) {
        if (!firstZero) {
          headerHits++;
          if (headerHits == headerBits) {
            digitalWrite(ledPin, 1);
          }
        }
        else {
          add(bitState);
        }
      }
      else {
        if (headerHits < headerBits) {
          noErrors = false;
        }
        else {
          if ((!firstZero) && (headerHits >= headerBits)) {
            firstZero = true;
          }
          add(bitState);
        }
      }
    }
  }
  digitalWrite(ledPin, 0);
}
 
void hexBinDump() {
  for ( int i = 0; i < maxBytes; i++) {
    byte mask = B10000000;
    if (manchester[i] < 16) {
    }
    for (int k = 0; k < 8; k++) {
      if (manchester[i] & mask) {
        Spaquet += '1'; 
        Spaquet2 += '1'; 
      }
      else {
        Spaquet += '0';
        Spaquet2 += '0';
      }
      mask = mask >> 1;
    }
    Spaquet2 += ' ';

  }
}
 
 void managedHideki(word p)
 {
		if (p != 0) 
		{
			//get pinData
			PulsePinData = p & 1;

//			if((p>400)&&(p<600)) Serial.print(0);else Serial.print(1);
//			Serial.println(p );

			if (tfa3208.nextPulse(p))
			{
				for (byte i=0;i<tfa3208.total_bits/8;i++) Serial.print(tfa3208.data[i],BIN); //		Serial.print(tfa3208.data[i],HEX);
                Serial.print(" ");

				for (byte i=0;i<tfa3208.total_bits/8;i++) Serial.print(tfa3208.data[i],HEX); //		Serial.print(tfa3208.data[i],HEX);

                // Serial.println(" ");
				Serial.print(" TFA      : ");
				Serial.print(tfa3208.GetCanal());
				Serial.print(" " );
				Serial.print(tfa3208.GetTemp());
				Serial.print(" " );
				Serial.println(tfa3208.GetHum());
				tfa3208.resetDecoder(); 
			}
		}
 }
void analyseData() {
  // Décodage propre aux satellites TFA Dostmann 30.3208.02
  char cTp[5];
 
  if (manchester[0] == 81) {// ce sont bien nos sondes (signature, identification dans le 1er octet du header
  
	//Serial.println(fifo.PWr);
	//Serial.println(fifo.PRd);

    cli();

	fifo.PRd = fifo.PWr+1;
	 if (fifo.PRd >=  SIZE_FIFO ) fifo.PRd=0;
	word p = 1 ;
	while(p!=0)
	{
		p = fifo.get();
		managedHideki(p);
	}
	//Serial.println();
	fifo.clear();
    sei();

	
//    Serial.println(Spaquet2);

    Serial.print(Spaquet.substring(0 , 11)); Serial.print(' ' );
    Serial.print(Spaquet.substring(11, 15)); Serial.print(' ' );
    Serial.print(Spaquet.substring(15, 19)); Serial.print(' ' );
    Serial.print(Spaquet.substring(19, 20)); Serial.print(' ' );
    Serial.print(Spaquet.substring(20, 23)); Serial.print(' ' );
    Serial.print(Spaquet.substring(23, 35)); Serial.print(' ' );
    Serial.print(Spaquet.substring(35, 43)); Serial.print(' ' );
    Serial.print(Spaquet.substring(43    )); Serial.print(' ' );
//    Serial.println();
	
    iCanal = Bin2Dec(Spaquet.substring(19, 22));
    fTemp = (Bin2Dec(Spaquet.substring(22, 34)) - 720) * 0.0556;
    fTemp *= 10;
    fTemp = int(fTemp + 0.5);
    fTemp /= 10;
    iHum = Bin2Dec(Spaquet.substring(34, 42));
    Serial.print("Décodage : ");
    Serial.print(iCanal);
    Serial.print(" " );
    Serial.print(fTemp);
    Serial.print(" " );
    Serial.println(iHum);
    Spaquet = "";
    Spaquet2 = "";
    //À ce stade, c'est décodé
    if (Nr == 0) { // 1ère réception, mise en mémoire
      Premier.Canal = iCanal;
      Premier.Temp = fTemp;
      Premier.Hum = iHum;
 
      Nr = 1;
    } else { // 2ème réception,
      if (iCanal == Premier.Canal ) { // Même satellite
        if (fTemp == Premier.Temp && iHum == Premier.Hum) {
          // Ok, Transmission
          mydata.Canal = iCanal ;
          // correction étalonnage
          mydata.Temp = fTemp + EtalonT[iCanal] ;
          mydata.Hum = iHum + EtalonH[iCanal];
 /*
          Serial.print(mydata.Canal);
          Serial.print(" " );
          Serial.print(mydata.Temp);
          Serial.print(" " );
          Serial.print(mydata.Hum);
          //ET.sendData(4); // vers le module (esclave) 4
          Serial.println(" transmis");
*/
          Nr = 0;
        } else {
          // pas identique, on reprend à zéro
          Nr = 0;
 
        }
      }// fin bon satellite
      else {
        // Pas le bon satellite
        Premier.Canal = iCanal ;
        Premier.Temp = fTemp ;
        Premier.Hum = iHum ;
      }
    }// fin 2nd réception
 
  }
}
 
void add(byte bitData) {
  if (discards > 0) { //if first one, it has not been found previously
    discards--;
  }
  else {
    dataByte = (dataByte << 1) | bitData;
    nosBits++;
    if (nosBits == 8) {
      nosBits = 0;
      manchester[nosBytes] = dataByte;
      nosBytes++;
    }
    if (nosBytes == maxBytes) {
      hexBinDump();
      analyseData();//later on develop your own analysis routines
    }
  }
}
 
void eraseManchester() {
  //Clear the memory to non matching numbers across the banks
  //If there is only one packet, with no repeats this is not necessary.
 
  for ( int i = 0; i < 20; i++) {
    manchester[i] = 0;
  }
}
 
int Bin2Dec(String Sbin) {
  int iDec = 0;
  int iBit = 0;
  int iRang = 0;
  for (int i = Sbin.length(); i >= 1; i--) {
    iBit = Sbin.substring(i - 1, i).toInt();
    iDec += (iBit * int(pow(2, iRang) + .5));
    iRang++;
  }
  return iDec;
}
