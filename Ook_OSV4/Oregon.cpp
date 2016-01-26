#include "arduino.h"

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
    return (data[3] & 0x4) ? 10 : 90;
}
 
byte channel(const byte* data)
{
    byte channel;
    switch (data[2])
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
	byte sId = data[2] && 0xff ;
	sId =  sId <<4 ;
	sId += data[3]>>4;
  return sId;
}
word getSensor (const byte* data)
{
	word sId = data[0];
	sId =  sId <<8 ;
	sId += data[1];
	return sId; 
}
