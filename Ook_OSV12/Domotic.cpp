#if _MSC_VER 
#include "vspde.h"
#endif

#include "arduino.h"
#include "Domotic.h"

#include "Oregon.h"
#include "Types.h"
#include "DecodeHomeEasy.h"
#include  "reportSerial.h"


tRBUF Cmd ;
unsigned long  lastTime;  
byte    receiveLength ;
byte    LastZoneValue[4];

byte    LastSensorValue[256] ;

byte    Seqnbr ;

//true if packet as been received and not treated
bool    DomoticPacketReceived;

typedef union tPAR {
byte Byte[4] ;
unsigned long Long;
}_tPAR;

unsigned long getLightingId (){
tPAR v  ;
v.Byte[0]=Cmd.LIGHTING2.id4 ;
v.Byte[1]=Cmd.LIGHTING2.id3 ;
v.Byte[2]=Cmd.LIGHTING2.id2 ;
v.Byte[3]=Cmd.LIGHTING2.id1 ;
return v.Long;
}

void resetLastSensorValue()
{
	memset(LastSensorValue,0,sizeof(LastSensorValue));
}
inline  void SetLastSensorValue(byte SensorId, byte sensorValue)
{
	LastSensorValue[SensorId]=sensorValue;
}
inline  byte GetLastSensorValue(byte SensorId)
{
	return LastSensorValue[SensorId];
}
inline  bool AsSensorValueChanged(byte SensorId, byte sensorValue)
{
	if ( LastSensorValue[SensorId]!=sensorValue)
	{
			LastSensorValue[SensorId]=sensorValue;
			return true;
	}
	return false;
}


void DomoticInit(){
lastTime =  0;
receiveLength = 0;
Cmd.LIGHTING2.packetlength  =0;
LastZoneValue[0];
LastZoneValue[1];
LastZoneValue[2];
LastZoneValue[3];
Seqnbr=0;
DomoticPacketReceived = false;

}

/// return true if packet en cours de reception
bool DomoticReceptionInProgress()
{
    return receiveLength != 0 ;
}
//reception d'une commande de send domotic
void DomoticReceive()
{
	//si paquet en cours de traitement 
	if (DomoticPacketReceived )
		return;

	byte nbCar = Serial.available();
	if (nbCar)
	{
        byte b = Serial.read();
        if ( (millis()-lastTime) > 10000 ) {
                receiveLength = 0 ;
        }
        lastTime = millis();
				if (receiveLength >= sizeof(_tRBUF)) {
					receiveLength = 0;
				}
        Cmd.Buffer[receiveLength++]=b;

				//if packet len not valid , wait valid
				if (    (Cmd.LIGHTING2.packetlength != 13 )
					   && (Cmd.LIGHTING2.packetlength != 11)
					)
					receiveLength = 0;

				/* si paquet recu en entier */
				if (receiveLength >= Cmd.LIGHTING2.packetlength + 1) {
					DomoticPacketReceived=  true;
					receiveLength = 0;
				}
				else
					DomoticPacketReceived=  false;

    }
}    

tRBUF Send ;

//envoi temperature : humudit a domotic
//, byte bateryLevel : 0..15 0 = batteri low
void reportDomoticTemp(int temp, byte id1, byte id2, byte bateryLevel) {

    if(isReportSerial())
    {
    }
    else
    {
        Send.Temp.packetlength = 8;
        Send.Temp.packettype = 0x50;
        Send.Temp.subtype = sTypeTEMP5;
        Send.Temp.seqnbr = Seqnbr++;
        Send.Temp.id1 = id1;
        Send.Temp.id2 = id2;

        if (temp >= 0)
            Send.Temp.tempsign = 0;
        else {
            Send.Temp.tempsign = 1;
            temp = -temp;
        }
        Send.Temp.temperatureh = temp >> 8;
        Send.Temp.temperaturel = temp & 0xff;
        Send.Temp.battery_level = bateryLevel;
        Send.Temp.rssi = 9;

        Serial.write((byte*)&Send.Temp, 9);
    }
}

//temp = temperature * 10 
void reportDomoticTempHum(int temp, byte hum, byte id1, byte id2, byte bateryLevel, byte subType) {

    if (isReportSerial())
    {
    }
    else
    {
        Send.Temp_Hum.packetlength = sizeof(Send.Temp_Hum) - 1;
        Send.Temp_Hum.packettype = 0x52;   //pTypeTEMP_HUM
        Send.Temp_Hum.subtype = subType;   //sTypeTH1 0x1  //THGN122/123,THGN132,THGR122/228/238/268
        Send.Temp_Hum.seqnbr = Seqnbr++;
        Send.Temp_Hum.id1 = id1;
        Send.Temp_Hum.id2 = id2;

        if (temp >= 0)
            Send.Temp_Hum.tempsign = 0;
        else {
            Send.Temp_Hum.tempsign = 1;
            temp = -temp;
        }

        Send.Temp_Hum.temperatureh = temp >> 8;
        Send.Temp_Hum.temperaturel = temp & 0xff;
        Send.Temp_Hum.battery_level = bateryLevel;
        Send.Temp_Hum.rssi = 9;
        Send.Temp_Hum.humidity = hum;
        Send.Temp_Hum.humidity_status = 0;

        Serial.write((byte*)&Send.Temp_Hum, sizeof(Send.Temp_Hum));
    }
}
void reportDomoticPower(byte* data, int size) {
    T_INT  tint;
    T_LONG tlong;

    if (isReportSerial())
    {
    }
    else
    {
        tint.Int = getPower(data);
        tlong.Long = getTotalPower(data);

        Send.ENERGY.packetlength = sizeof(Send.ENERGY) - 1;
        Send.ENERGY.packettype = pTypeENERGY;
        Send.ENERGY.subtype = sTypeELEC2; //0x1   //CM119/160 
        Send.ENERGY.seqnbr = Seqnbr++;
        Send.ENERGY.id1 = data[0];
        Send.ENERGY.id2 = data[1];
        Send.ENERGY.battery_level = 15;
        Send.ENERGY.rssi = 9;
        Send.ENERGY.count = 1;
        Send.ENERGY.instant1 = 0;
        Send.ENERGY.instant2 = 0;
        Send.ENERGY.instant3 = tint.Car[1];
        Send.ENERGY.instant4 = tint.Car[0];
        Send.ENERGY.total1 = 0;
        Send.ENERGY.total2 = 0;
        Send.ENERGY.total3 = tlong.Car[3];
        Send.ENERGY.total4 = tlong.Car[2];
        Send.ENERGY.total5 = tlong.Car[1];
        Send.ENERGY.total6 = tlong.Car[0];

        Serial.write((byte*)&Send.Temp_Hum, sizeof(Send.ENERGY));
    }
}

void reportDomoticTempBaro (byte id1 , float temp , float baro , float altitude , uint8_t forecast )
{
    if (isReportSerial())
    {
    }
    else
    {

        Send._tTempBaro.len = sizeof(Send._tTempBaro) - 1;
        Send._tTempBaro.type = pTypeTEMP_BARO;
        Send._tTempBaro.subtype = sTypeBMP085;
        Send._tTempBaro.id1 = id1;
        Send._tTempBaro.temp = temp;
        Send._tTempBaro.baro = baro;
        Send._tTempBaro.altitude = altitude;
        Send._tTempBaro.forecast = forecast;
        Serial.write((byte*)&Send._tTempBaro, sizeof(Send._tTempBaro));
    }
}

//status types for humidity
#define humstat_normal 0x0
#define humstat_comfort 0x1
#define humstat_dry 0x2
#define humstat_wet 0x3

unsigned char Get_Humidity_Level(const unsigned char hlevel)
{
	if (hlevel < 25)
		return humstat_dry;
	if (hlevel > 60)
		return humstat_wet;
	if ((hlevel >= 25) && (hlevel <= 60))
		return humstat_comfort;
	return humstat_normal;
}

void reportDomoticTempHumBaro (byte id1 , byte unit ,float temperature , float pressure , uint8_t forecast , byte humidity , byte BatteryLevel , byte RssiLevel  )
{
    if (isReportSerial())
    {
    }
    else
    {

        Send.TEMP_HUM_BARO.packetlength = sizeof(Send.TEMP_HUM_BARO) - 1;
        Send.TEMP_HUM_BARO.packettype = pTypeTEMP_HUM_BARO;
        Send.TEMP_HUM_BARO.subtype = sTypeTHB1;
        Send.TEMP_HUM_BARO.battery_level = BatteryLevel;
        Send.TEMP_HUM_BARO.rssi = RssiLevel;
        Send.TEMP_HUM_BARO.id1 = id1;
        Send.TEMP_HUM_BARO.id2 = unit;

        Send.TEMP_HUM_BARO.tempsign = (temperature >= 0) ? 0 : 1;
        int at10 = (abs(temperature * 10.0F));
        Send.TEMP_HUM_BARO.temperatureh = (byte)(at10 / 256);
        at10 -= (Send.TEMP_HUM_BARO.temperatureh * 256);
        Send.TEMP_HUM_BARO.temperaturel = (byte)(at10);
        Send.TEMP_HUM_BARO.humidity = (byte)humidity;
        Send.TEMP_HUM_BARO.humidity_status = Get_Humidity_Level(humidity);

        int ab10 = round(pressure);
        Send.TEMP_HUM_BARO.baroh = (byte)(ab10 / 256);
        ab10 -= (Send.TEMP_HUM_BARO.baroh * 256);
        Send.TEMP_HUM_BARO.barol = (byte)(ab10);

        Send.TEMP_HUM_BARO.forecast = (byte)forecast;

        Serial.write((byte*)&Send.TEMP_HUM_BARO, sizeof(Send.TEMP_HUM_BARO));
    }
}

void reportDomotic ( const byte* data,byte size ){

	//id1 = sensor id id2 = channel pour oregon
  if (getSensorByte1(data)==CMR180_ID1)
    reportDomoticPower ( (byte*)data, size );
  else
    //homeEasy sensor
  if (getSensorByte1(data)==HOMESWITCH_ID0)
    reportDomoticHomeEasy( (byte*)data, size );
  else
    reportDomoticTempHum (temperatureint(data) , getHumidity(data) , getId(data),channel(data), battery(data), sTypeTH1_OREGON );
	
}

void DomoticStartReceive()
{
  Send.IRESPONSE.packetlength = sizeof(Send.IRESPONSE)-1;
  Send.IRESPONSE.packettype = pTypeInterfaceMessage;
  Send.IRESPONSE.subtype = cmdStartRec ;
  Send.IRESPONSE.msg1 ='C';
  Send.IRESPONSE.msg2 ='o';
  Send.IRESPONSE.msg3 ='p';
  Send.IRESPONSE.msg4 ='y';
  Send.IRESPONSE.msg5 ='r';
  Send.IRESPONSE.msg6 ='i';
  Send.IRESPONSE.msg7 ='g';
  Send.IRESPONSE.msg8 ='h';
  Send.IRESPONSE.msg9 ='t';
  Send.IRESPONSE.msg10=' ';
  Send.IRESPONSE.msg11='R';
  Send.IRESPONSE.msg12='F';
  Send.IRESPONSE.msg13='X';
  Send.IRESPONSE.msg14='C';
  Send.IRESPONSE.msg15='O';
  Send.IRESPONSE.msg16='M';
  Serial.write((byte*)&Send.IRESPONSE,sizeof(Send.IRESPONSE));

}
void DomoticStatus()
{
  Send.IRESPONSE.packetlength = sizeof(Send.IRESPONSE)-1;
  Send.IRESPONSE.packettype = pTypeInterfaceMessage;
  Send.IRESPONSE.subtype = sTypeInterfaceResponse ;
  Send.IRESPONSE.cmnd    = cmdSTATUS ;
  Send.IRESPONSE.msg1    = trxType43392 ;
  Send.IRESPONSE.msg2    = VERSION  ;    //Firmware version
  Send.IRESPONSE.msg3    = 0  ;
  Send.IRESPONSE.msg4    = 0  ;
  Send.IRESPONSE.msg5    = 0  ;
  Send.IRESPONSE.msg6    = 1  ;    //Hardware version
  Send.IRESPONSE.msg7    = 0  ;    //Hardware version
		
	
  Serial.write((byte*)&Send.IRESPONSE,sizeof(Send.IRESPONSE));
	
}

void reportHagerDomoticUnk ( const byte* data, byte pos ){
  Send.UNDECODED.packetlength = pos+3;
  Send.UNDECODED.packettype = pTypeUndecoded;
  Send.UNDECODED.subtype = 0xff ;

  for (byte i = 0; i < pos; ++i) {
      Send.UNDECODED.msg[i]= data[i];
  }
  Serial.write((byte*)&Send.UNDECODED,Send.UNDECODED.packetlength+1  );
}

extern byte GetMode(const byte* data) ;
extern byte GetZone(const byte* data);
#define HORS_GEL  0x4 
#define CONFOR    0x0
#define ECO       0x3
#define ARRET     0x5

void reportHagerDomoticSerial(const byte* data, byte pos) {
    if (isReportSerial())
    {
    }
    else
    {
        Send.LIGHTING2.packetlength = 11;
        Send.LIGHTING2.packettype = pTypeLighting2;
        Send.LIGHTING2.subtype = sTypeANSLUT;							//AC
        Send.LIGHTING2.seqnbr = Seqnbr++;
        Send.LIGHTING2.id1 = data[0];            /* id emetteur 0..3  */
        Send.LIGHTING2.id2 = data[1];            /* id emetteur 0..FF */
        Send.LIGHTING2.id3 = data[2];            /* id emetteur 0..FF */
        Send.LIGHTING2.id4 = 0;            /* id emetteur 0..FF */
        Send.LIGHTING2.unitcode = GetZone(data);  		/* unit = zone 1..3  */
        Send.LIGHTING2.level = 0;   /* dim level 0..15   */
        Send.LIGHTING2.rssi = 0;
        //devbug
        /*
            Send.LIGHTING2.packetlength = 11 + 4; //+4 debug
                Send.LIGHTING2.data[0]  = data[3];
                Send.LIGHTING2.data[1]  = data[4];
                Send.LIGHTING2.data[2]  = data[5];
                Send.LIGHTING2.data[3]  = data[6];
        */

        if (GetMode(data) == CONFOR)
            Send.LIGHTING2.cmnd = 1;
        else
            Send.LIGHTING2.cmnd = 0;
        Serial.write((byte*)&Send.LIGHTING2, Send.LIGHTING2.packetlength + 1);
    }
}
void reportHagerDomotic(const byte* data, byte pos) {
    byte zone = GetZone(data);
    byte cmnd;
    if (GetMode(data) == CONFOR)
        cmnd = 1;
    else
        cmnd = 0;

    if (LastZoneValue[zone] != cmnd)
    {
        LastZoneValue[zone] = cmnd;
        reportHagerDomoticSerial(data, pos);
    }
}
void reportDomoticHomeEasy(const byte* data, byte pos) {
    if (isReportSerial())
    {
    }
    else
    {


        Send.LIGHTING2.packetlength = 11;
        Send.LIGHTING2.packettype = pTypeLighting2;
        Send.LIGHTING2.subtype = sTypeHEU;
        Send.LIGHTING2.seqnbr = Seqnbr++;
        Send.LIGHTING2.id1 = data[5];            /* id emetteur 0..3  */
        Send.LIGHTING2.id2 = data[6];            /* id emetteur 0..FF */
        Send.LIGHTING2.id3 = data[7];            /* id emetteur 0..FF */
        Send.LIGHTING2.id4 = data[7];            /* id emetteur 0..FF */
        Send.LIGHTING2.unitcode = data[2];        		/* unit = zone 1..3  */
        Send.LIGHTING2.level = 0;   /* dim level 0..15   */
        Send.LIGHTING2.rssi = 0;
        //devbug
        /*
                Send.LIGHTING2.packetlength = 11 + 4; //+2 debug
                Send.LIGHTING2.data[0]  = data[3];
                Send.LIGHTING2.data[1]  = data[4];
                Send.LIGHTING2.data[2]  = data[5];
                Send.LIGHTING2.data[3]  = data[6];
        */
        Send.LIGHTING2.cmnd = data[3];

        Serial.write((byte*)&Send.LIGHTING2, Send.LIGHTING2.packetlength + 1);
    }
}
void reportDomoticMD230(const byte* data, byte pos) {

    //  MD230:2E DB 0B 44 10   //ok
    //	MD230 : 2E DB 0B 04 50    //alarm 1
    //	MD230 : 2E DB 0B 14 40   //alarm 2
    //	MD230 : 2E DB 0B 14 40
    //	MD230 : 2E DB 0B 54 00
    if (isReportSerial())
    {
    }
    else
    {

        Send.LIGHTING2.packetlength = 11;
        Send.LIGHTING2.packettype = pTypeLighting2;
        Send.LIGHTING2.subtype = sTypeHEU;
        Send.LIGHTING2.seqnbr = Seqnbr++;
        Send.LIGHTING2.id1 = 0;                 /* id emetteur 0..3  */
        Send.LIGHTING2.id2 = data[0];            /* id emetteur 0..FF */
        Send.LIGHTING2.id3 = data[1];            /* id emetteur 0..FF */
        Send.LIGHTING2.id4 = data[2];            /* id emetteur 0..FF */
        Send.LIGHTING2.unitcode = 1;         	 /* unit = zone 1..3  */
        Send.LIGHTING2.level = 0;   /* dim level 0..15   */
        Send.LIGHTING2.rssi = 0;

        if ((data[3] == 0x14) && (data[4] == 0x40))
            Send.LIGHTING2.cmnd = 0;
        else
            Send.LIGHTING2.cmnd = 1;

        Serial.write((byte*)&Send.LIGHTING2, Send.LIGHTING2.packetlength + 1);
    }
}


