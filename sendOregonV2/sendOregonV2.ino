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
*/
 
// #define THN132N
 
const byte TX_PIN = 3;

#define LED_PIN 9
 
const unsigned long TIME = 512;
const unsigned long TWOTIME = TIME*2;
 
#define SEND_HIGH() digitalWrite(TX_PIN, HIGH)
#define SEND_LOW() digitalWrite(TX_PIN, LOW)
 
// Buffer for Oregon message

#define CMR180_ID0 0x3A
#define CMR180_ID1 0x80

#ifdef THN132N
  #define DATA_SIZE 8  
#else
  #define DATA_SIZE 9
#endif

byte OregonMessageBuffer[10];
  
typedef union T_int {
char Car[2] ;
int  Int;
}T_INT;

typedef union T_long {
char Car[4] ;
long Long ;
}T_LONG;

/**
 * \brief    Send logical "0" over RF
 * \details  azero bit be represented by an off-to-on transition
 * \         of the RF signal at the middle of a clock period.
 * \         Remenber, the Oregon v2.1 protocol add an inverted bit first 
 */
inline void sendZero(void) 
{
  SEND_HIGH();
  delayMicroseconds(TIME);
  SEND_LOW();
  delayMicroseconds(TWOTIME);
  SEND_HIGH();
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
   SEND_LOW();
   delayMicroseconds(TIME);
   SEND_HIGH();
   delayMicroseconds(TWOTIME);
   SEND_LOW();
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
#ifdef THN132N
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
inline void setType(byte *data, byte typeByte0 , byte typeByte1 ) 
{
  data[0] = typeByte0;
  data[1] = typeByte1;
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
void calculateAndSetChecksum(byte* data, byte dataSize )
{
#ifdef THN132N
    int s = ((Sum(6, data) + (data[6]&0xF) - 0xa) & 0xff);
 
    data[6] |=  (s&0x0F) << 4;     data[7] =  (s&0xF0) >> 4;
#else
    data[dataSize-1] = ((Sum(dataSize-1, data) - 0xa) & 0xFF);
#endif
}


struct T_POWER {
char Device[2] ;
char PacketType[1] ;
int  Power ;
long TotalPower;
};

/*****************************************************************
data [0.1] device type
data [2  ] packet type 0   : power + total power
                  type 1..4: power 
data [3..4] power : 2 byte watt
data [5..8] total power : 4 byte watt /s 

*/
/**
 * \brief    power instantané en watt
 * \param    data       Oregon message
 * \param    power     
 */
void setPower(byte* data, int  power)
{
  T_INT tint ;
  tint.Int = power ;
  data[3] = tint.Car[0] ;
  data[4] = tint.Car[1] ;
}
int getPower(byte* data )
{
  T_INT tint ;
  tint.Car[0] = data[3] ;
  tint.Car[1] = data[4] ;
  return tint.Int  ;
   
}
/**
 * \brief    total power en watt/s
 * \param    data       Oregon message
 * \param    totalpower     
 */
void setTotalPower(byte* data, long  totalpower)
{
  T_LONG tint ;
  tint.Long = totalpower ;
  data[5] = tint.Car[0] ;
  data[6] = tint.Car[1] ;
  data[7] = tint.Car[2] ;
  data[8] = tint.Car[3] ;
}
long getTotalPower(byte* data)
{
  T_LONG tint  ;
  tint.Car[0] = data[5];
  tint.Car[1] = data[6];
  tint.Car[2] = data[7];
  tint.Car[3] = data[8];
  return tint.Long ;
}

void setPowerPacketType(byte* data, byte packetType)
{
  data[0] = CMR180_ID0 ;
  data[1] = CMR180_ID1 ;
  data[2] = packetType ;
}
byte getPowerPacketType(byte* data)
{
  return data[2];
}
 



/******************************************************************/
/******************************************************************/
float Temp ; 
void setup()
{
  pinMode(TX_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
 
  Serial.begin(9600);
  Serial.println("\n[Oregon V2.1 encoder]");
 
  SEND_LOW();  
 
#ifdef THN132N  
  // Create the Oregon message for a temperature only sensor (TNHN132N)
  byte ID[] = {0xEA,0x4C};
#else
  // Create the Oregon message for a temperature/humidity sensor (THGR2228N)
  byte ID[] = {0x1A,0x2D};
#endif  
 
  setType(OregonMessageBuffer, ID);
  setChannel(OregonMessageBuffer, 0x20);
  setId(OregonMessageBuffer, 0xBB);
  Temp=11.2;
}

void PrintMessage(byte* data, byte dataSize )
{
  for (byte i = 0; i < dataSize ; ++i)   {     
    Serial.print(data[i] >> 4, HEX);
    Serial.print(data[i] & 0x0F, HEX);
  }
 Serial.println();
 
}

void reportSerial ( byte * data, byte pos ) {
// Energy OWL : CMR180
    if ( (data[0] == CMR180_ID0 ) && (data[1] == CMR180_ID1 ) )
    {
       Serial.print("[CMR180,...] Id:");
       Serial.print(data[0], HEX);Serial.print(data[1], HEX);
       Serial.print(", size:");
       Serial.print(pos);
       Serial.print(" ,power:");
       Serial.print(getPower(data)); 
       if (pos > 6) {
         // Display only for main frame
         // Secondary frame is only 6 Bytes long
         Serial.print(" ,total:");
         Serial.print(getTotalPower(data));
         Serial.print(" ,total kWh:");
         Serial.print(getTotalPower(data)/3600/1000);
       }
       Serial.println();
    }
}

void PulseLed(byte ledPin)
{
      if (digitalRead(ledPin)== LOW) 
         digitalWrite(ledPin, HIGH);  
      else
         digitalWrite(ledPin, LOW);  
}


void loop()
{
  // Get Temperature, humidity and battery level from sensors
  // (ie: 1wire DS18B20 for temp�rature, ...)
  setBatteryLevel(OregonMessageBuffer, 0); // 0 : low, 1 : high
  setTemperature(OregonMessageBuffer, Temp);
  Temp += 0.1 ;
 
#ifndef THN132N
  // Set Humidity
  setHumidity(OregonMessageBuffer, 52);
#endif  
 
  // Calculate the checksum
  calculateAndSetChecksum(OregonMessageBuffer,DATA_SIZE);
 
  // Show the Oregon Message
  PrintMessage(OregonMessageBuffer,DATA_SIZE);
 
  // Send the Message over RF
  sendOregon(OregonMessageBuffer, DATA_SIZE );
  // Send a "pause"
  SEND_LOW();
  delayMicroseconds(TWOTIME*8);
  // Send a copie of the first message. The v2.1 protocol send the
  // message two time 
  sendOregon(OregonMessageBuffer, DATA_SIZE );
 
  // Wait for 30 seconds before send a new message 
  SEND_LOW();
  delay(30000);
 PulseLed(LED_PIN);
  
}
