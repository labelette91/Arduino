
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
		
		byte    receiveLength ;
	} LIGHTING2;

byte Buffer[sizeof(struct T_LIGHTING2)] ;
	
};	
extern  tRBUF Cmd ;

extern bool DomoticReceive();
extern void reportDomotic ( const byte* data);
extern void reportDomoticTempHum ( const byte* data);
extern unsigned long getLightingId ();
extern void  DomoticInit();
extern void reportDomotic ( int temp , byte id1 , byte id2 );
extern void reportDomoticTempHum ( int temp , byte hum , byte id1 , byte id2 );

