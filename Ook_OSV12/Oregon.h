#pragma once 

extern int Sum(byte count, const byte* data);
extern byte checksum(const byte* data);
extern byte checksum2(const byte* data);
extern byte checksum(const byte* data,byte size );

extern float temperature(const byte* data);
       //temperature sur un int multiplier par 10
extern int temperatureint(const byte* data);
extern int temperatureEnt(const byte* data);
extern int temperatureFrac(const byte* data);
extern byte getHumidity(const byte* data);
       // Ne retourne qu'un apercu de l'etat de la baterie : 10 = faible
extern byte battery(const byte* data);
extern byte channel(const byte* data);
extern byte getOrId (const byte* data);
extern word getSensor (const byte* data);

#define CMR180_ID0 0x3A
#define CMR180_ID1 0x80

byte getPowerPacketType(byte *data);
unsigned long getTotalPower(byte *data);
int  getPower(byte *data);

#define getSensorByte0(data)(data[0])
#define getSensorByte1(data)(data[1])

