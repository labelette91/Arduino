#include "arduino.h"
#include "sendOregon.h"

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
 

#define LED_PIN 9
 
 
// Buffer for Oregon message


#ifdef THN132N
  #define DATA_SIZE 8  
#else
  #define DATA_SIZE 9
#endif

typedef union T_int {
char Car[2] ;
int  Int;
}T_INT;

typedef union T_long {
char Car[4] ;
long Long ;
}T_LONG;

 
/**
 * \brief    Send an Oregon message
 * \param    data   The Oregon message
 */
void sendOregon::sendBuffer( byte size)
{
    SEND_LOW();
    sendPreamble();
    //sendSync();
    sendData(data, size);
    sendPostamble();
    SEND_LOW();
  
}
void sendOregon::Send( byte size)
{
    sendBuffer(  size);
    // Send a "pause"
    delayMicroseconds(TWOTIME*8);
    sendBuffer(  size);
}

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

void sendOregon::CalculateAndSetChecksum(byte dataSize) 
{
  calculateAndSetChecksum( data, dataSize );
}


/******************************************************************/
/******************************************************************/
/******************************************************************/
 
/**
 * \brief    Set the sensor type
 * \param    data       Oregon message
 * \param    type       Sensor type
 */
void setType(byte *data, byte* type) 
{
  data[0] = type[0];
  data[1] = type[1];
}
void setType(byte *data, byte typeByte0 , byte typeByte1 ) 
{
  data[0] = typeByte0;
  data[1] = typeByte1;
}
 
/**
 * \brief    Set the sensor channel
 * \param    data       Oregon message
 * \param    channel    Sensor channel (0x10, 0x20, 0x30)
 */
void setChannel(byte *data, byte channel) 
{
    data[2] = channel;
}
 
/**
 * \brief    Set the sensor ID
 * \param    data       Oregon message
 * \param    ID         Sensor unique ID
 */
void setId(byte *data, byte ID) 
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
 
