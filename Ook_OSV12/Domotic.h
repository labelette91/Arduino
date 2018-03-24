#define VERSION 14

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
		byte	data[2] ;
		
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
		byte	rssi : 4;
		byte	battery_level : 4;
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
	
}_tRBUF;	

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


extern  tRBUF Cmd ;

extern bool DomoticReceive();
extern void reportDomoticTemp ( const byte* data);
extern void reportDomoticTempHum ( const byte* data);
extern unsigned long getLightingId ();
extern void  DomoticInit();
extern void reportDomoticTemp ( int temp , byte id1 , byte id2 , byte bateryLevel);
extern void reportDomoticTempHum ( int temp , byte hum , byte id1 , byte id2, byte bateryLevel);
extern void DomoticStartReceive();
extern void DomoticStatus();
void reportHagerDomotic ( const byte* data, byte pos );
void resetLastSensorValue();
void reportDomotic ( const byte* data,byte size );
void reportDomoticHomeEasy ( const byte* data, byte pos );
void reportDomoticMD230(const byte* data, byte pos);


