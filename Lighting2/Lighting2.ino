#if _MSC_VER 
#include "debug\vspde.h"
#endif

typedef union tRBUF {
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
tRBUF Cmd ;
unsigned long  lastTime;  

bool DomoticReceive()
{
    if (Serial.available()){
        byte b = Serial.read();
        if ( (millis()-lastTime) > 500 ) {
                Cmd.LIGHTING2.receiveLength = 0 ;
                Cmd.LIGHTING2.packetlength  =0;
        }
        lastTime = millis();
        if (Cmd.LIGHTING2.receiveLength<Cmd.LIGHTING2.packetlength+1) 
            Cmd.Buffer[Cmd.LIGHTING2.receiveLength++]=b;
        /* si paquet recu en entier */
        if (Cmd.LIGHTING2.receiveLength>=Cmd.LIGHTING2.packetlength+1) {
            /* verification packettype */
            if (Cmd.LIGHTING2.packettype==17) {
                long Id = (( unsigned long  )Cmd.LIGHTING2.id1) << 24 +
                          (( unsigned long  )Cmd.LIGHTING2.id2) << 16 +
                          (( unsigned long  )Cmd.LIGHTING2.id3) <<  8 +
                          (( unsigned long  )Cmd.LIGHTING2.id4)       ;
                return true;
            }
            else{
                Cmd.LIGHTING2.receiveLength = 0 ;
                Cmd.LIGHTING2.packetlength  =0;
            }
        }
    }
    return false ;
}    


void setup()
{
    Serial.begin(38400);
    Cmd.LIGHTING2.receiveLength = 0 ;
    Cmd.LIGHTING2.packetlength  =0;
}

void loop()
{
  
  if (DomoticReceive())
  {
  	Blink(9,500);
    Serial.print("Id:" );
    Serial.print(Cmd.LIGHTING2.id1,HEX ) ;
    Serial.print(Cmd.LIGHTING2.id2,HEX ) ;
    Serial.print(Cmd.LIGHTING2.id3,HEX ) ;
    Serial.print(Cmd.LIGHTING2.id4,HEX ) ;
    Serial.print("code:" );
    Serial.print(Cmd.LIGHTING2.unitcode,HEX ) ;
    Serial.print(":" );
    Serial.print(Cmd.LIGHTING2.cmnd,DEC ) ;
    
  }
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}


