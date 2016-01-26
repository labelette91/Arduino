#include "arduino.h"

#define CMR180_ID0 0x3A
#define CMR180_ID1 0x80

const unsigned long TIME = 512;
const unsigned long TWOTIME = TIME*2;
 
#define SEND_HIGH() digitalWrite(TX_PIN, HIGH)
#define SEND_LOW() digitalWrite(TX_PIN, LOW)

void setPowerPacketType(byte *data,byte packetType);
void setTotalPower(byte *data,long totalpower);
void setPower(byte *data,int power);

void setHumidity(byte *data,byte hum);
void setTemperature(byte *data,float temp);
void setBatteryLevel(byte *data,byte level);
void setId(byte *data,byte ID);
void setChannel(byte *data,byte channel);
void setType(byte *data,byte *type);
void setType(byte *data,byte typeByte0,byte typeByte1);

byte getPowerPacketType(byte *data);
long getTotalPower(byte *data);
int  getPower(byte *data);


class sendOregon{

  byte TX_PIN ;
  byte data[10];

public:
  sendOregon(byte pin ) {
    TX_PIN=pin;
    pinMode(TX_PIN, OUTPUT);
    SEND_LOW();  

  };
  sendOregon() {
    TX_PIN=3;
    pinMode(TX_PIN, OUTPUT);
    SEND_LOW();  
  };
  void Send(byte size);
  byte * Data(){return data;};
  void CalculateAndSetChecksum(byte size) ;

  void SetPowerPacketType (byte packetType){setPowerPacketType (data, packetType);};
  void SetTotalPower      (long totalpower){setTotalPower      (data, totalpower);};
  void SetPower           (int power)      {setPower           (data, power);};
  void SetHumidity        (byte hum)       {setHumidity        (data, hum);};
  void SetTemperature     (float temp)     {setTemperature     (data, temp);};
  void SetBatteryLevel    (byte level)     {setBatteryLevel    (data, level);};
  void SetId              (byte ID)        {setId              (data, ID);};
  void SetChannel         (byte channel)   {setChannel         (data, channel);};
  void SetType            (byte *type)     {setType            (data, type);};
  void SetType            (byte typeByte0,byte typeByte1){setType            (data,typeByte0,typeByte1);};
  byte GetPowerPacketType ()                {getPowerPacketType(data);};
  long GetTotalPower      ()                {getTotalPower     (data);};
  int  GetPower           ()                {getPower          (data);};


private:
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
/**
 * \brief    Sum data for checksum
 * \param    count      number of bit to sum
 * \param    data       Oregon message
 */


/**
 * \brief    Send a buffer over RF
 * \param    data   Data to send
 * \param    size   size of data to send
 */
inline void sendData(byte *data, byte size)
{
  for(byte i = 0; i < size; ++i)
  {
    sendQuarterLSB(data[i]);
    sendQuarterMSB(data[i]);
  }
}
void sendBuffer( byte size);
};

