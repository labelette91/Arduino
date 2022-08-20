#define VERSION 15


typedef union tRBUF {
//domotic structure
	struct T_TEMP {
		byte	packetlength;
		byte	packettype;
		byte	subtype;
		byte	seqnbr;
		byte	id1;
		byte	id2;
		byte	temperatureh : 7;
		byte	tempsign : 1;
		byte	temperaturel;
		byte	battery_level : 4;
		byte	rssi : 4;
	} Temp;
	
	
	struct T_TEMP_HUM{
		byte	packetlength;
		byte	packettype;
		byte	subtype;
		byte	seqnbr;
		byte	id1;
		byte	id2;
		byte	temperatureh : 7;
		byte	tempsign : 1;
		byte	temperaturel;
		byte	humidity; 
		byte	humidity_status;
		byte	battery_level : 4;
		byte	rssi : 4;
	} Temp_Hum;
	
	
	
struct  T_LIGHTING2{
		byte	packetlength;   /* == 11 : longueur a partir de packettype */
		byte	packettype;     /* 17    */
		byte	subtype;
		byte	seqnbr;
		byte	id1;            /* id emetteur 0..3  */
		byte	id2;            /* id emetteur 0..FF */
		byte	id3;            /* id emetteur 0..FF */
		byte	id4;            /* id emetteur 0..FF */
		byte	unitcode;       /* unit 0.0xF        */
		byte	cmnd;           /* 0 : off 1:On      */
		byte	level;          /* dim level 0..15   */
		byte	rssi ;
//		byte	data[2] ;
		
	} LIGHTING2;

byte Buffer[sizeof(struct T_LIGHTING2)] ;

		struct {
		byte	packetlength; // = 13
		byte	packettype;
		byte	subtype;
		byte	seqnbr;
		byte	cmnd;
		byte	msg1;
		byte	msg2;
		byte	msg3;
		byte	msg4;
		byte	msg5;
		byte	msg6;
		byte	msg7;
		byte	msg8;
		byte	msg9;
	} ICMND;

struct {	//response on a mode command from the application
		byte	packetlength; //= 20
		byte	packettype;
		byte	subtype;
		byte	seqnbr;
		byte	cmnd;
		byte	msg1;	//receiver/transceiver type   #define trxType43392 0x53
		byte	msg2;	//firmware version
		byte	msg3;
		byte	msg4;
		byte	msg5;
		byte	msg6;
		byte	msg7;
		byte	msg8;
		byte	msg9;
		byte	msg10;
    byte	msg11;
    byte	msg12;
    byte	msg13;
    byte	msg14;
    byte	msg15;
    byte	msg16;
	} IRESPONSE;
	
	struct {
		byte	packetlength;
		byte	packettype;
		byte	subtype;
		byte	seqnbr;
		byte	msg[17];
	} UNDECODED;

	struct {
		byte	packetlength;
		byte	packettype;
		byte	subtype;
		byte	seqnbr;
		byte	id1;
		byte	id2;
		byte	count;
		byte	ch1h;
		byte	ch1l;
		byte	ch2h;
		byte	ch2l;
		byte	ch3h;
		byte	ch3l;
		byte	rssi : 4;
		byte	battery_level : 4;
	} CURRENT;

	struct {
		byte	packetlength;
		byte	packettype;
		byte	subtype;
		byte	seqnbr;
		byte	id1;
		byte	id2;
		byte	count;
		byte	instant1;
		byte	instant2;
		byte	instant3;
		byte	instant4;
		byte	total1;
		byte	total2;
		byte	total3;
		byte	total4;
		byte	total5;
		byte	total6;
		byte	battery_level : 4;
		byte	rssi : 4;
	} ENERGY;

	struct {
		byte	packetlength;
		byte	packettype;
		byte	subtype;
		byte	seqnbr;
		byte	id1;
		byte	id2;
		byte	count;
		byte	ch1h;
		byte	ch1l;
		byte	ch2h;
		byte	ch2l;
		byte	ch3h;
		byte	ch3l;
		byte	total1;
		byte	total2;
		byte	total3;
		byte	total4;
		byte	total5;
		byte	total6;
		byte	rssi : 4;
		byte	battery_level : 4;
	} CURRENT_ENERGY;

 struct 
{
	uint8_t len;
	uint8_t type;
	uint8_t subtype;
	uint8_t id1;
	float   temp;
	float   baro;
	float   altitude;
	uint8_t forecast;

} _tTempBaro;

#define BYTE byte
 	struct {
		BYTE	packetlength;
		BYTE	packettype;
		BYTE	subtype;
		BYTE	seqnbr;
		BYTE	id1;
		BYTE	id2;
#ifdef IS_BIG_ENDIAN
		BYTE	tempsign : 1;
		BYTE	temperatureh : 7;
#else
		BYTE	temperatureh : 7;
		BYTE	tempsign : 1;
#endif
		BYTE	temperaturel;
		BYTE	humidity;
		BYTE	humidity_status;
		BYTE	baroh;
		BYTE	barol;
		BYTE	forecast;
#ifdef IS_BIG_ENDIAN
		BYTE	rssi : 4;
		BYTE	battery_level : 4;
#else
		BYTE	battery_level : 4;
		BYTE	rssi : 4;
#endif
	} TEMP_HUM_BARO;


}_tRBUF;	

#define pTypeTEMP_BARO 0xF7
#define sTypeBMP085 0x01




#define cmdRESET	  0x00 // reset the receiver/transceiver
#define cmdSTATUS	  0x02 // return firmware versions and configuration of the interface
#define cmdStartRec	0x07 // start RFXtrx receiver

#define trxType43392 0x53

#define pTypeInterfaceMessage 0x01
#define sTypeInterfaceResponse 0x00

#define sTypeInterfaceCommand 0x00


#define pTypeLighting2 0x11
#define sTypeAC 0x0
#define sTypeHEU 0x1
#define sTypeANSLUT 0x2
#define sTypeKambrook 0x03

#define pTypeUndecoded 0x03


//types for current
#define pTypeCURRENT 0x59
#define sTypeELEC1 0x1   //CM113,Electrisave

//types for energy
#define pTypeENERGY 0x5A
#define sTypeELEC2 0x1   //CM119/160
#define sTypeELEC3 0x2   //CM180

//types for current-energy
#define pTypeCURRENTENERGY 0x5B
#define sTypeELEC4 0x1   //CM180i

//types for power
#define pTypePOWER 0x5C
#define sTypeELEC5 0x1   //revolt

//types for temperature+humidity
#define pTypeTEMP_HUM 0x52
#define sTypeTH1_OREGON 0x1  //THGN122/123,THGN132,THGR122/228/238/268
#define sTypeTH2 0x2  //THGR810,THGN800
#define sTypeTH3 0x3  //RTGR328
#define sTypeTH4 0x4  //THGR328
#define sTypeTH5 0x5  //WTGR800
#define sTypeTH6 0x6  //THGR918,THGRN228,THGN500
#define sTypeTH7 0x7  //TFA TS34C, Cresta
#define sTypeTH8 0x8  //WT450H
#define sTypeTH9 0x9  //Viking 02035,02038 (02035 has no humidity), TSS320
#define sTypeTH10_RUBiCSON 0xA   //Rubicson
#define sTypeTH11_TFA 0xB   //EW109
#define sTypeTH12 0xC   //Imagintronix
#define sTypeTH13 0xD   //Alecto WS1700 and compatibles
#define sTypeTH14 0xE   //Alecto

//types for temperature
#define pTypeTEMP 0x50
#define sTypeTEMP1 0x1  //THR128/138,THC138, Davis
#define sTypeTEMP2 0x2  //THC238/268,THN132,THWR288,THRN122,THN122,AW129/131
#define sTypeTEMP3 0x3  //THWR800
#define sTypeTEMP4 0x4	//RTHN318
#define sTypeTEMP5 0x5  //LaCrosse TX3
#define sTypeTEMP6 0x6  //TS15C
#define sTypeTEMP7 0x7  //Viking 02811,TSS330
#define sTypeTEMP8 0x8  //LaCrosse WS2300
#define sTypeTEMP9 0x9  //RUBiCSON
#define sTypeTEMP10_TFA 0xA  //TFA 30.3133
#define sTypeTEMP11 0xB  //WT0122

#define pTypeTEMP_HUM_BARO 0x54
#define sTypeTHB1 0x1   //BTHR918,BTHGN129
#define sTypeTHB2 0x2   //BTHR918N,BTHR968
#define sTypeTHBFloat 0x10	    // Weather Station

extern  tRBUF Cmd ;
extern bool DomoticPacketReceived;

bool DomoticReceptionInProgress();

extern void DomoticReceive();
extern unsigned long getLightingId ();
extern void  DomoticInit();
extern void reportDomoticTemp ( int temp , byte id1 , byte id2 , byte bateryLevel);
extern void reportDomoticTempHum ( int temp , byte hum , byte id1 , byte id2, byte bateryLevel, byte subType);
extern void DomoticStartReceive();
extern void DomoticStatus();
void reportHagerDomotic ( const byte* data, byte pos );
void resetLastSensorValue();
void reportDomoticTempBaro(byte id1, float temp, float baro, float altitude, uint8_t forecast);
void reportDomoticTempHumBaro(byte id1, byte unit, float temperature, float pressure, uint8_t forecast, byte humidity, byte BatteryLevel, byte RssiLevel);
void reportDomotic ( const byte* data,byte size );
void reportDomoticHomeEasy ( const byte* data, byte pos );
void reportDomoticMD230(const byte* data, byte pos);


