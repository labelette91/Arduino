
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
	
};	

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


extern  tRBUF Cmd ;

extern bool DomoticReceive();
extern void reportDomotic ( const byte* data);
extern void reportDomoticTempHum ( const byte* data);
extern unsigned long getLightingId ();
extern void  DomoticInit();
extern void reportDomotic ( int temp , byte id1 , byte id2 );
extern void reportDomoticTempHum ( int temp , byte hum , byte id1 , byte id2 );
extern void DomoticStartReceive();
extern void DomoticStatus();
void reportHagerDomotic ( const byte* data, byte pos );


