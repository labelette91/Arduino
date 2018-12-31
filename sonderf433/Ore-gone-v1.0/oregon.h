
typedef union s_Word {
  byte b[2];
  unsigned int i;
} Word;
 
typedef struct s_Sensor {
    Word Type;
    byte Channel;
    byte UniqueID;
    byte Flags;
    byte MessageSize;
    byte BytesToChecksum;
    byte MessageBuffer[15];
    byte Checksum;
    byte LowBattery;
} Sensor;


void initCrc8(void);
void crc8(unsigned char *crc, unsigned char m);
inline void sendPreamble(byte protocol);
int Sum(byte count, const byte* data);
void PrepareSensor(Sensor *TheSensor, byte protocol);
byte calculateAndSetChecksum(Sensor TheSensor);
void setTemperature(Sensor *TheSensor, float temp);
void setHumidity(Sensor *TheSensor, byte hum);
unsigned char calcCrc(Sensor TheSensor);
void sendOregon(byte *data, byte size, byte protocol);
void PrepareSensor(Sensor *TheSensor, byte protocol);
void setTemperature(Sensor *TheSensor, float temp);
void setHumidity(Sensor *TheSensor, byte hum);

// Oregon timings 
#define  TIME       512
#define  TWOTIME    TIME*2
#define  TIMEV3     512

#define TX_PIN      10
 
#define SEND_HIGH() digitalWrite(TX_PIN, HIGH)
#define SEND_LOW() digitalWrite(TX_PIN, LOW)

#define OREGONV2    0
#define OREGONV3    1

// Oregon IDs
#define TEMP_HYGRO_OREGON        0x1A2D   // RTGR238N inside temp + hygro

#define OREGON_DEFAULT_UNIQUE_ID  0xEE

// Oregon Message structure (in nibbles)
// Preamble (4/6) - Sync (1) - Payload (variable) - Checksum (2) - Post-amble (2/5)
// Payload detail :
// Sensor ID (4) - Channel (1) - Rolling Code (2) - Flags (1) - Data (variable) - Checksum (2)

// Temp + Hygro Packet
// ID (4) + Channel (1) - Rolling Code - Flag
// Temperature LSB in 0.1 degC - Temperature (int part) - Temperature Sign Non-zero for negative values - Relative Humidity Percent - ??



// crc constants
#define GP  0x107   /* x^8 + x^2 + x + 1 */
#define DI  0x07


