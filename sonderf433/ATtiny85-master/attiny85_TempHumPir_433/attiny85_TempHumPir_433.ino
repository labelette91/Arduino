/*
 * connectingStuff, Oregon Scientific v2.1 Emitter
 * http://connectingstuff.net/blog/encodage-protocoles-oregon-scientific-sur-arduino/
 *
 * Copyright (C) 2013 olivier.lebrun@gmail.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * V2 par vil1driver
 * 
 * sketch unique pour sonde ds18b20 ou DHT11/22
 * choix de la périodicité de transmission
 * remontée niveau de batterie
 * 
 * ajout d'au capteur pir ou reed ou tilt
 *
*/

/************************************************************

    emplacement des PIN de la puce ATtiny8
    
                +-------+
Ain0  D5  PB5  1|*      |8   VCC
Ain3  D3  PB3  2|       |7   PB2  D2  Ain1
Ain2  D4  PB4  3|       |6   PB1  D1  pwm1
          GND  4|       |5   PB0  D0  pwm0
                +-------+ 


            cablage a realiser
                
                +-------+
               1|*      |8   (+)
  Data Sonde   2|       |7
      TX 433   3|       |6   Switch B (optionel)
         (-)   4|       |5   Switch A (optionel)
                +-------+ 

                              
****************      Confuguration     *******************/


#define NODE_ID 0xCC              // Identifiant unique de votre sonde (hexadecimal)
#define LOW_BATTERY_LEVEL 2600    // Voltage minumum (mV) avant d'indiquer batterie faible
#define WDT_COUNT 5              // Nombre de cycles entre chaque mesure (1 cycles = 8 secondes, 5x8 = 40s)

// decommenter la ligne qui corresponds a votre sonde
#define DS18B20
//#define DHT11
//#define DHT22

// si une mesure est identique a la precedente, elle ne sera pas transmise
// on economise ainsi la batterie
// decommentez la ligne suivante si vous souhaitez transmettre chaque mesure
//#define ALWAYS_SEND


/**********************************************************/


// decommenter la(les) ligne(s) suivante(s) si vous utilisez un(des) capteur(s) supplementaire(s)
//#define SWITCH_A
//#define SWITCH_B

#define SWITCH_A_HOUSE_CODE 'E'        // code maison du capteur A
#define SWITCH_A_UNIT_CODE 6           // code unite du capteur A
#define SWITCH_B_HOUSE_CODE 'E'        // code maison du capteur B
#define SWITCH_B_UNIT_CODE 7           // code unite du capteur B


/**********************************************************/


#define DATA_PIN 3                // pin 2 // data de la sonde
#define TX_PIN 4                  // pin 3 // data transmetteur
#define SWITCH_A_PIN 0            // pin 5 // wake up SWITCH A output
#define SWITCH_B_PIN 1            // pin 6 // wake up SWITCH B output


/***************  Fin de configuration   *****************/


// Chargement des librairies
#include <avr/sleep.h>    // Sleep Modes
#include <avr/wdt.h>      // Watchdog timer
#include <avr/interrupt.h>
#ifdef DS18B20
  #include "OneWire.h"
  #define DS18B20 0x28     // Adresse 1-Wire du DS18B20
  OneWire ds(DATA_PIN); // Création de l'objet OneWire ds
#else
  #include "dht.h"
  dht DHT;
#endif
#if defined(SWITCH_A) || defined(SWITCH_B)
  #include  "x10rf.h"
  x10rf myx10 = x10rf(TX_PIN,0,3); // no blink led and send msg three times
#endif

#ifndef cbi
  #define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
  #define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#ifdef SWITCH_A
  volatile uint8_t oldValueA = -1; // for x10 switch A
#endif
#ifdef SWITCH_B
  volatile uint8_t oldValueB = -1; // for x10 switch B
#endif  
volatile float lastTemp = 0.0;
volatile int count = 0;
boolean lowBattery = false;
const unsigned long TIME = 512;
const unsigned long TWOTIME = TIME*2;

#define SENDHIGH() digitalWrite(TX_PIN, HIGH)
#define SENDLOW() digitalWrite(TX_PIN, LOW)
 
// Buffer for Oregon message
#ifdef DS18B20
  byte OregonMessageBuffer[8];
#else
  byte OregonMessageBuffer[9];
#endif
 
/**
 * \brief    Send logical "0" over RF
 * \details  azero bit be represented by an off-to-on transition
 * \         of the RF signal at the middle of a clock period.
 * \         Remenber, the Oregon v2.1 protocol add an inverted bit first 
 */
inline void sendZero(void) 
{
  SENDHIGH();
  delayMicroseconds(TIME);
  SENDLOW();
  delayMicroseconds(TWOTIME);
  SENDHIGH();
  delayMicroseconds(TIME);
}
 
/**
 * \brief    Send logical "1" over RF
 * \details  a one bit be represented by an on-to-off transition
 * \         of the RF signal at the middle of a clock period.
 * \         Remenber, the Oregon v2.1 protocol add an inverted bit first 
 */
inline void sendOne(void) 
{
   SENDLOW();
   delayMicroseconds(TIME);
   SENDHIGH();
   delayMicroseconds(TWOTIME);
   SENDLOW();
   delayMicroseconds(TIME);
}
 
/**
* Send a bits quarter (4 bits = MSB from 8 bits value) over RF
*
* @param data Source data to process and sent
*/
 
/**
 * \brief    Send a bits quarter (4 bits = MSB from 8 bits value) over RF
 * \param    data   Data to send
 */
inline void sendQuarterMSB(const byte data) 
{
  (bitRead(data, 4)) ? sendOne() : sendZero();
  (bitRead(data, 5)) ? sendOne() : sendZero();
  (bitRead(data, 6)) ? sendOne() : sendZero();
  (bitRead(data, 7)) ? sendOne() : sendZero();
}
 
/**
 * \brief    Send a bits quarter (4 bits = LSB from 8 bits value) over RF
 * \param    data   Data to send
 */
inline void sendQuarterLSB(const byte data) 
{
  (bitRead(data, 0)) ? sendOne() : sendZero();
  (bitRead(data, 1)) ? sendOne() : sendZero();
  (bitRead(data, 2)) ? sendOne() : sendZero();
  (bitRead(data, 3)) ? sendOne() : sendZero();
}
 
/******************************************************************/
/******************************************************************/
/******************************************************************/
 
/**
 * \brief    Send a buffer over RF
 * \param    data   Data to send
 * \param    size   size of data to send
 */
void sendData(byte *data, byte size)
{
  for(byte i = 0; i < size; ++i)
  {
    sendQuarterLSB(data[i]);
    sendQuarterMSB(data[i]);
  }
}
 
/**
 * \brief    Send an Oregon message
 * \param    data   The Oregon message
 */
void sendOregon(byte *data, byte size)
{
    sendPreamble();
    //sendSync();
    sendData(data, size);
    sendPostamble();
}
 
/**
 * \brief    Send preamble
 * \details  The preamble consists of 16 "1" bits
 */
inline void sendPreamble(void)
{
  byte PREAMBLE[]={0xFF,0xFF};
  sendData(PREAMBLE, 2);
}
 
/**
 * \brief    Send postamble
 * \details  The postamble consists of 8 "0" bits
 */
inline void sendPostamble(void)
{
#ifdef DS18B20
  sendQuarterLSB(0x00);
#else
  byte POSTAMBLE[]={0x00};
  sendData(POSTAMBLE, 1);  
#endif
}
 
/**
 * \brief    Send sync nibble
 * \details  The sync is 0xA. It is not use in this version since the sync nibble
 * \         is include in the Oregon message to send.
 */
inline void sendSync(void)
{
  sendQuarterLSB(0xA);
}
 
/******************************************************************/
/******************************************************************/
/******************************************************************/
 
/**
 * \brief    Set the sensor type
 * \param    data       Oregon message
 * \param    type       Sensor type
 */
inline void setType(byte *data, byte* type) 
{
  data[0] = type[0];
  data[1] = type[1];
}
 
/**
 * \brief    Set the sensor channel
 * \param    data       Oregon message
 * \param    channel    Sensor channel (0x10, 0x20, 0x30)
 */
inline void setChannel(byte *data, byte channel) 
{
    data[2] = channel;
}
 
/**
 * \brief    Set the sensor ID
 * \param    data       Oregon message
 * \param    ID         Sensor unique ID
 */
inline void setId(byte *data, byte ID) 
{
  data[3] = ID;
}
 
/**
 * \brief    Set the sensor battery level
 * \param    data       Oregon message
 * \param    level      Battery level (0 = low, 1 = high)
 */
void setBatteryLevel(byte *data, byte level)
{
  if(!level) data[4] = 0x0C;
  else data[4] = 0x00;
}
 
/**
 * \brief    Set the sensor temperature
 * \param    data       Oregon message
 * \param    temp       the temperature
 */
void setTemperature(byte *data, float temp) 
{
  // Set temperature sign
  if(temp < 0)
  {
    data[6] = 0x08;
    temp *= -1;  
  }
  else
  {
    data[6] = 0x00;
  }
 
  // Determine decimal and float part
  int tempInt = (int)temp;
  int td = (int)(tempInt / 10);
  int tf = (int)round((float)((float)tempInt/10 - (float)td) * 10);
 
  int tempFloat =  (int)round((float)(temp - (float)tempInt) * 10);
 
  // Set temperature decimal part
  data[5] = (td << 4);
  data[5] |= tf;
 
  // Set temperature float part
  data[4] |= (tempFloat << 4);
}
 
/**
 * \brief    Set the sensor humidity
 * \param    data       Oregon message
 * \param    hum        the humidity
 */
void setHumidity(byte* data, byte hum)
{
    data[7] = (hum/10);
    data[6] |= (hum - data[7]*10) << 4;
}
 
/**
 * \brief    Sum data for checksum
 * \param    count      number of bit to sum
 * \param    data       Oregon message
 */
int Sum(byte count, const byte* data)
{
  int s = 0;
 
  for(byte i = 0; i<count;i++)
  {
    s += (data[i]&0xF0) >> 4;
    s += (data[i]&0xF);
  }
 
  if(int(count) != count)
    s += (data[count]&0xF0) >> 4;
 
  return s;
}
 
/**
 * \brief    Calculate checksum
 * \param    data       Oregon message
 */
void calculateAndSetChecksum(byte* data)
{
#ifdef DS18B20
    int s = ((Sum(6, data) + (data[6]&0xF) - 0xa) & 0xff);
 
    data[6] |=  (s&0x0F) << 4;     data[7] =  (s&0xF0) >> 4;
#else
    data[8] = ((Sum(8, data) - 0xa) & 0xFF);
#endif
}
 
/******************************************************************/
/******************************************************************/


// Fonction récupérant la température
// Retourne true si tout va bien, ou false en cas d'erreur
boolean getTemperature(float *temp){

#ifdef DS18B20  
  byte present = 0;
  byte data[9];
  byte addr[8];
  // data : Données lues depuis le scratchpad
  // addr : adresse du module 1-Wire détecté

  if (!ds.search(addr)) { // Recherche un module 1-Wire
  ds.reset_search();    // Réinitialise la recherche de module
  delay(250);
  //return false;         // Retourne une erreur
  }

  if (OneWire::crc8(addr, 7) != addr[7]) // Vérifie que l'adresse a été correctement reçue
  return false;                        // Si le message est corrompu on retourne une erreur

  if (addr[0] != DS18B20) // Vérifie qu'il s'agit bien d'un DS18B20
  return false;         // Si ce n'est pas le cas on retourne une erreur
  
  ds.reset();             // On reset le bus 1-Wire
  ds.select(addr);        // On sélectionne le DS18B20

  ds.write(0x44, 1);      // On lance une prise de mesure de température
  delay(1000);             // Et on attend la fin de la mesure

  present = ds.reset();             // On reset le bus 1-Wire
  ds.select(addr);        // On sélectionne le DS18B20
  ds.write(0xBE);         // On envoie une demande de lecture du scratchpad

  for (byte i = 0; i < 9; i++) // On lit le scratchpad
  data[i] = ds.read();       // Et on stock les octets reçus

  // Calcul de la température en degré Celsius
  *temp = ((data[1] << 8) | data[0]) * 0.0625; 
  
  // Pas d'erreur
  return true;
#else
  #ifdef DHT11
    //delay(2000);
    
    int chk = DHT.read11(DATA_PIN);
    
    if (chk == DHTLIB_OK) { // Ok
      // Pas d'erreur
      *temp = DHT.temperature;
      return true;
    }
    else
    {
      return false;
    }
  #else 
    #ifdef DHT22
      //delay(2000);
      
      int chk = DHT.read22(DATA_PIN);
      
      if (chk == DHTLIB_OK) { // Ok
        // Pas d'erreur
        *temp = DHT.temperature;
        return true;
      
      }
      else
      {
        return false;
      }  
    #endif
  #endif
#endif  
}

 
/******************************************************************/
 
void setup()
{

 CLKPR = (1<<CLKPCE);  
 CLKPR = B00000000;  // set the fuses to 8mhz clock-speed.
 
#ifdef SWITCH_A
   pinMode(SWITCH_A_PIN, INPUT);
   PCMSK |= bit (PCINT0);
#endif   
#ifdef SWITCH_B
   pinMode(SWITCH_B_PIN, INPUT);
   PCMSK |= bit (PCINT1);
#endif 
#if defined(SWITCH_A) || defined(SWITCH_B)   
   GIFR |= bit (PCIF);  // clear any outstanding interrupts
   GIMSK |= bit (PCIE); // enable pin change interrupts 
   sei();               // enable interrupts   
#endif
 
 #if defined(DS18B20) || defined(DHT11) || defined(DHT22)
    // initialisation des cycles de reveil
    setup_watchdog(9);
 #endif
 
 pinMode(TX_PIN, OUTPUT); // sortie transmetteur

  SENDLOW();  
 
#ifdef DS18B20  
  // Create the Oregon message for a temperature only sensor (TNHN132N)
  byte ID[] = {0xEA,0x4C};
#else
  // Create the Oregon message for a temperature/humidity sensor (THGR2228N)
  byte ID[] = {0x1A,0x2D};
#endif  
 
  setType(OregonMessageBuffer, ID);
  setChannel(OregonMessageBuffer, 0x20);
  setId(OregonMessageBuffer, NODE_ID);

  delay(2000);

}


// set system into the sleep state 
// system wakes up when wtchdog is timed out
void system_sleep() {
  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_mode();                        // Go to sleep
  sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON
}

// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void setup_watchdog(int ii) {
  byte bb;
  int ww;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);
  ww=bb;

  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}
  
// Watchdog Interrupt Service / is executed when watchdog timed out 
ISR(WDT_vect) {   
  //wake up
  count--;
} 

#if defined(SWITCH_A) || defined(SWITCH_B)
  // PIN Interrupt Service
  ISR(PCINT0_vect) 
  {
      //wake up
      delay(10); // debounce
  }
#endif


//reads internal 1V1 reference against VCC
//return number 0 .. 1023 
int analogReadInternal() {
  ADMUX = _BV(MUX3) | _BV(MUX2); // For ATtiny85
  delay(5); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  uint8_t low = ADCL;
  return (ADCH << 8) | low; 
}

//calculate VCC based on internal referrence
//return voltage in mV
int readVCC() {
  return ((uint32_t)1024 * (uint32_t)1100) / analogReadInternal();
}


void loop()
{
#if defined(DS18B20) || defined(DHT11) || defined(DHT22)
  if (count <= 0) { // on attend que le nombre de cycle soit atteint
      
      count=WDT_COUNT;  // reset counter
      
      // Get Temperature, humidity and battery level from sensors
      float temp; 
      
      if (getTemperature(&temp)) {


          // we need round temp to one decimal...
          int a = round(temp * 10);
          temp = a / 10.0;
  
          // if temp has changed
          if (temp != lastTemp) {
          
              #ifndef ALWAYS_SEND
                // save temp
                lastTemp = temp;
              #endif  
              
              // Get the battery state
              int vcc = readVCC();
              lowBattery = vcc < LOW_BATTERY_LEVEL;
              
              // Set Battery Level
              setBatteryLevel(OregonMessageBuffer, !lowBattery);  // 0=low, 1=high
              
              // Set Temperature
              setTemperature(OregonMessageBuffer, temp);
              
              #ifndef DS18B20
                // Set Humidity
                setHumidity(OregonMessageBuffer, DHT.humidity);
              #endif
                
              // Calculate the checksum
              calculateAndSetChecksum(OregonMessageBuffer);
               
              // Send the Message over RF
              sendOregon(OregonMessageBuffer, sizeof(OregonMessageBuffer));
              // Send a "pause"
              SENDLOW();
              delayMicroseconds(TWOTIME*8);
              // Send a copie of the first message. The v2.1 protocol send the message two time 
              sendOregon(OregonMessageBuffer, sizeof(OregonMessageBuffer));
              SENDLOW();
                  }   
          }
  }
#endif

  #ifdef SWITCH_A
    // Get the update value
    uint8_t valueA = (digitalRead(SWITCH_A_PIN)==HIGH ? OFF : ON);
     
    if (valueA != oldValueA) {
       // Send in the new value
       myx10.x10Switch(SWITCH_A_HOUSE_CODE,SWITCH_A_UNIT_CODE,valueA);
       oldValueA = valueA;
    }
  #endif
  #ifdef SWITCH_B
    // Get the update value
    uint8_t valueB = (digitalRead(SWITCH_B_PIN)==HIGH ? OFF : ON);
     
    if (valueB != oldValueB) {
       // Send in the new value
       myx10.x10Switch(SWITCH_B_HOUSE_CODE,SWITCH_B_UNIT_CODE,valueB);
       oldValueB = valueB;
    }
  #endif
    
  system_sleep();
}
