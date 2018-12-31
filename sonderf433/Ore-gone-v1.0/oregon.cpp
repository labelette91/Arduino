#include <arduino.h>
#include "oregon.h"

unsigned char m_crc8Table[256];     /* 8-bit table */

// Oregon V2.1 doubles each manchester bit and sends the inverted one first 
inline void sendZero(void)
{
  SEND_HIGH();
  delayMicroseconds(TIME);
  SEND_LOW();
  delayMicroseconds(TWOTIME);
  SEND_HIGH();
  delayMicroseconds(TIME);
}
 
inline void sendOne(void)
{
   SEND_LOW();
   delayMicroseconds(TIME);
   SEND_HIGH();
   delayMicroseconds(TWOTIME);
   SEND_LOW();
   delayMicroseconds(TIME);
}
 
// Oregon V3 sends a single manchester bit (non inverted) 
inline void sendOneV3(void)
{
  SEND_HIGH();
  delayMicroseconds(TIMEV3);
  SEND_LOW();
  delayMicroseconds(TIMEV3);
} 

inline void sendZeroV3(void)
{
  SEND_LOW();
  delayMicroseconds(TIMEV3);
  SEND_HIGH();
  delayMicroseconds(TIMEV3);
} 


///////////////////////////////////////////////////////////////////////////

void sendData(byte *data, byte size)
{
  for(byte i = 0; i < size; ++i)
  {
    // LSB nibble first
    (bitRead(data[i], 0)) ? sendOne() : sendZero();
    (bitRead(data[i], 1)) ? sendOne() : sendZero();
    (bitRead(data[i], 2)) ? sendOne() : sendZero();
    (bitRead(data[i], 3)) ? sendOne() : sendZero();
    
    // MSB nibble
    (bitRead(data[i], 4)) ? sendOne() : sendZero();
    (bitRead(data[i], 5)) ? sendOne() : sendZero();
    (bitRead(data[i], 6)) ? sendOne() : sendZero();
    (bitRead(data[i], 7)) ? sendOne() : sendZero();
  }
}
 
 
 
void sendDataV3(byte *data, byte size)
{
  for(byte i = 0; i < size; ++i)
  {
    // LSB nibble first
    (bitRead(data[i], 0)) ? sendOneV3() : sendZeroV3();
    (bitRead(data[i], 1)) ? sendOneV3() : sendZeroV3();
    (bitRead(data[i], 2)) ? sendOneV3() : sendZeroV3();
    (bitRead(data[i], 3)) ? sendOneV3() : sendZeroV3();
    
    // MSB nibble
    (bitRead(data[i], 4)) ? sendOneV3() : sendZeroV3();
    (bitRead(data[i], 5)) ? sendOneV3() : sendZeroV3();
    (bitRead(data[i], 6)) ? sendOneV3() : sendZeroV3();
    (bitRead(data[i], 7)) ? sendOneV3() : sendZeroV3();
  }
} 
 
void sendOregon(byte *data, byte size, byte protocol)
{
    sendPreamble(protocol);
    if(protocol == OREGONV2)
    {
      sendData(data, size);
      sendData(0x00, 1);
    }
    else
    {
      sendDataV3(data, size);
      sendDataV3(0x00, 1);
    }
}
 
inline void sendPreamble(byte protocol)
{
  byte PREAMBLE[]={0xFF,0xFF, 0xFF};
  if(protocol == OREGONV2)
    sendData(PREAMBLE, 2);
  else
  {
    sendDataV3(PREAMBLE, 3);  
  }
}


void setTemperature(Sensor *TheSensor, float temp)
{
  // Set temperature sign
  if(temp < 0)
  {
    TheSensor->MessageBuffer[6] = 0x08;
    temp *= -1; 
  }
  else
  {
    TheSensor->MessageBuffer[6] = 0x00;
  }
 
  // Determine decimal and float part
  int tempInt = (int)temp;
  int td = (int)(tempInt / 10);
  int tf = (int)round((float)((float)tempInt/10. - (float)td) * 10.);
 
  int tempFloat =  (int)round((float)(temp - (float)tempInt) * 10.);
 
  /*Serial.println("\n\ntd tf tempfloat");
  Serial.println(td);
  Serial.println(tf);
  Serial.println(tempFloat);*/
  
  
  // Set temperature decimal part
  TheSensor->MessageBuffer[5] = (td << 4);
  TheSensor->MessageBuffer[5] |= tf;
 
  // Set temperature float part and low battery bit
  if(TheSensor->LowBattery)
    TheSensor->MessageBuffer[4] = 0x0C;
  else
    TheSensor->MessageBuffer[4] = 0x00;
    
  TheSensor->MessageBuffer[4] |= (tempFloat << 4);
}
 

void setHumidity(Sensor *TheSensor, byte hum)
{
    TheSensor->MessageBuffer[7] = (hum/10);
    TheSensor->MessageBuffer[6] |= (hum - TheSensor->MessageBuffer[7]*10) << 4;
}

 
int Sum(byte count, const byte* data)
{
  int s = 0;
  int tempcount;
 
  for(byte i = 0; i<count;i++)
  {
    s += (data[i]&0xF0) >> 4;
    s += (data[i]&0xF);
  }

  tempcount = (int)count;
  if(tempcount != count)
  {
    s += (data[count]&0xF0) >> 4;
  }
  return s;
}
 

byte calculateAndSetChecksum(Sensor TheSensor)
{
  byte checksum;
  checksum = ((Sum(TheSensor.BytesToChecksum, TheSensor.MessageBuffer) - 0xa) & 0xFF);
  return(checksum);
}

unsigned char calcCrc(Sensor TheSensor)
{
  unsigned char crc = 0;

  // includes most payload bytes but does not include checksum
  for (int i=0; i < TheSensor.BytesToChecksum; i++)
  {
     crc8(&crc, TheSensor.MessageBuffer[i]);
  }
  return (crc);
}
 
 
// this CRC code is from crc8.c published by Rajiv Chakravorty
// Should be called before any other crc function.  
void initCrc8(void) 
{
  int i,j;
  unsigned char crc;
  
  for (i=0; i<256; i++) 
  {
    crc = i;
    for (j=0; j<8; j++)
    {
      crc = (crc << 1) ^ ((crc & 0x80) ? DI : 0);
    }
      
    m_crc8Table[i] = crc & 0xFF;
  }
}

// For a byte array whose accumulated crc value is stored in *crc, computes
// resultant crc obtained by appending m to the byte array
void crc8(unsigned char *crc, unsigned char m)
 
{
  *crc = m_crc8Table[(*crc) ^ m];
  *crc &= 0xFF;
} 
 
 
void PrepareSensor(Sensor *TheSensor, byte protocol)
{
  if(protocol == OREGONV2)
  {
    TheSensor->MessageBuffer[0] = TheSensor->Type.b[1];  // Type
    TheSensor->MessageBuffer[1] =  TheSensor->Type.b[0];
    TheSensor->MessageBuffer[2] = (1 << TheSensor->Channel);
    TheSensor->MessageBuffer[2] = (TheSensor->MessageBuffer[2] << 4) & 0xF0;  // 3-4 channel max in V2.1
    TheSensor->MessageBuffer[3] = TheSensor->UniqueID;
    TheSensor->MessageBuffer[4] = 0x00;    // clears the low battery bit
  }
 else
 {
    TheSensor->MessageBuffer[0] = 0x0A;
    TheSensor->MessageBuffer[0] |= TheSensor->Type.b[1] & 0xF0;  // Type
    TheSensor->MessageBuffer[1] = TheSensor->Type.b[1] & 0x0F;
    TheSensor->MessageBuffer[1] |= TheSensor->Type.b[0] & 0xF0;
    TheSensor->MessageBuffer[2] = TheSensor->Type.b[0] & 0x0F;
    TheSensor->MessageBuffer[2] |= ((TheSensor->Channel << 4) & 0xF0); // Ch 0-15, 4 bit coded channel on V3
    TheSensor->MessageBuffer[3] = TheSensor->UniqueID;
  }
}
