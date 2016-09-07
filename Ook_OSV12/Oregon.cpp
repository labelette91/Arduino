#include "arduino.h"
#include "Types.h"

/*
Message Formats
All messages decoded so far (versions 2.1 and 3.0) appear to have an identical
format for the sensor data payload, as shown in the table below. Figure 1
(earlier in this document) depicts the payload format. The message is assumed
to contain n nibbles, numbered from zero to n-1. For convenience, this table
also shows the checksum and post-amble portions of the message.

Nibble(s) 	Contents 	Details
0..3 				Sensor ID This 16-bit value is unique to each sensor,or sometimes a group of sensors.
4 					Channel 	Some sensors use the coding 1 << (ch  1),where ch is 1, 2 or 3.
5..6 				Rolling Code Value changes randomly every time the sensor is reset
7 					Flags 1 Bit value 0x4 is the battery low flag
8..[n-5] 		Sensor-specific Data Usually in BCD format
[n-3]..[n-4] Checksum The 8-bit sum of nibbles 0..[n-5]
[n-1]..[n-2] Post-amble Unknown contents and purpose

The coding of sensor-specific data varies according to the type of
measurements being reported by the sensor. Some sensors use the same coding
as others which report the similar data but this is not always the case. For
example, the THGR810 and THGR122NX temperature/humidity sensors use the
same data coding, but the RGR968 and PCR800 rain gauges do not.
Most (but not all) sensor data is in BCD format, least significant digit (LSD)
first. For example the value of 27.5 degrees Celsius would be coded as the hex
nibbles 572. The decimal point is assumed to be in a fixed location and is not
actually transmitted. 
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

byte checksum(const byte* data)
{
  int c = ((data[6]&0xF0) >> 4) + ((data[7]&0xF)<<4);
  int s = ((Sum(6, data) + (data[6]&0xF) - 0xa) & 0xff);
  return s == c;
}
 
byte checksum2(const byte* data)
{
  return data[8] == ((Sum(8, data) - 0xa) & 0xff);
}

//size = taille comprenant la checksum
byte checksum(const byte* data,byte size )
{
  return data[size-1] == ((Sum(size-1, data) - 0xa) & 0xff);
}


float temperature(const byte* data)
{
    int sign = (data[6]&0x8) ? -1 : 1;
    float temp = ((data[5]&0xF0) >> 4)*10 + (data[5]&0xF) + (float)(((data[4]&0xF0) >> 4) / 10.0);
    return sign * temp;
}
//temperature sur un int multiplier par 10
int temperatureint(const byte* data)
{
    int sign = (data[6]&0x8) ? -1 : 1;
    int temp = ((data[5]&0xF0) >> 4)*10 + (data[5]&0xF) ;
    temp = temp*10 + ((data[4]&0xF0) >> 4) ;
    return sign * temp;
}
int temperatureEnt(const byte* data)
{
    int sign = (data[6]&0x8) ? -1 : 1;
    int temp = ((data[5]&0xF0) >> 4)*10 + (data[5]&0xF) ;
    return sign * temp;
}
int temperatureFrac(const byte* data)
{
    int temp = ((data[4]&0xF0) >> 4) ;
    return temp;
}
byte getHumidity(const byte* data)
{
    return (data[7]&0xF) * 10 + ((data[6]&0xF0) >> 4);
}

// Ne retourne qu'un apercu de l'etat de la baterie : 10 = faible
byte battery(const byte* data)
{
    return (data[4] & 0x4) ? 10 : 90;
}
 
byte channel(const byte* data)
{
    byte channel;
    switch (data[2]& 0xF0)
    {
        case 0x10:
            channel = 1;
            break;
        case 0x20:
            channel = 2;
            break;
        case 0x40:
            channel = 3;
            break;
     }
 
     return channel;
}

byte getId (const byte* data)
{
	byte sId = data[2] && 0x0f ;
	sId =  sId <<4 ;
	sId += data[3]>>4;
  return sId;
//  return data[3];
}
word getSensor (const byte* data)
{
	word sId = data[0];
	sId =  sId <<8 ;
	sId += data[1];
	return sId; 
}



/*****************************************************************
power devive
data [0.1] device type
data [2  ] packet type 0   : power + total power
                  type 1..4: power 
data [3..4] power : 2 byte watt
data [5..8] total power : 4 byte watt /s 

*/
/**
 * \brief    power instantanÃ© en watt
 * \param    data       Oregon message
 * \param    power     
 */
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
long getTotalPower(byte* data)
{
  T_LONG tint  ;
  tint.Car[0] = data[5];
  tint.Car[1] = data[6];
  tint.Car[2] = data[7];
  tint.Car[3] = data[8];
  return tint.Long ;
}

byte getPowerPacketType(byte* data)
{
  return data[2];
}
 


