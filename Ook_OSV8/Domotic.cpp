#if _MSC_VER 
#include "debug\vspde.h"
#endif

#include "arduino.h"
#include "Domotic.h"

#include "Oregon.h"

tRBUF Cmd ;
unsigned long  lastTime;  
byte    receiveLength ;


unsigned long getLightingId (){
	                return   (( unsigned long  )Cmd.LIGHTING2.id1) << 24 +
                          (( unsigned long  )Cmd.LIGHTING2.id2) << 16 +
                          (( unsigned long  )Cmd.LIGHTING2.id3) <<  8 +
                          (( unsigned long  )Cmd.LIGHTING2.id4)       ;

}
void DomoticInit(){
lastTime =  0;
receiveLength = 0;
Cmd.LIGHTING2.packetlength  =0;
}
//reception d'une commande de send domotic
bool DomoticReceive()
{
    if (Serial.available()){
        byte b = Serial.read();
        if ( (millis()-lastTime) > 500 ) {
                receiveLength = 0 ;
                Cmd.LIGHTING2.packetlength  =0;
        }
        lastTime = millis();
        if (receiveLength<Cmd.LIGHTING2.packetlength+1) 
            Cmd.Buffer[receiveLength++]=b;
        /* si paquet recu en entier */
        if (receiveLength>=Cmd.LIGHTING2.packetlength+1) {
            receiveLength = 0 ;
            return true;
        }
    }
    return false ;
}    



tRBUF Send ;

//envoi temperature : humudit a domotic

void reportDomotic ( int temp , byte id1 , byte id2 ){

		Send.Temp.packetlength = 8;
		Send.Temp.packettype   = 0x50;
		Send.Temp.subtype      = 05  ;
		Send.Temp.seqnbr       = 0x2e;
		Send.Temp.id1          = id1 ;
		Send.Temp.id2          = id2 ;
		
		Send.Temp.temperatureh = temp >>8   ;
		if (temp>=0)
			Send.Temp.tempsign     = 0   ;
		else
			Send.Temp.tempsign     = 1   ;
		Send.Temp.temperaturel   = temp & 0xff;
		Send.Temp.battery_level  = 6;
		Send.Temp.rssi           = 9 ;
            
    Serial.write((byte*)&Send.Temp,9);
}
void reportDomotic ( const byte* data){
	reportDomotic ( temperatureint(data) , 0x48  , 0x00 );
	
}

void reportDomoticTempHum ( int temp , byte hum , byte id1 , byte id2 ){
	
  Send.Temp_Hum.packetlength = sizeof(Send.Temp_Hum)-1;
  Send.Temp_Hum.packettype   = 0x52;   //pTypeTEMP_HUM
  Send.Temp_Hum.subtype      = 01  ;   //sTypeTH1 0x1  //THGN122/123,THGN132,THGR122/228/238/268
  Send.Temp_Hum.seqnbr       = 0x2e;
  Send.Temp_Hum.id1          = id1;
  Send.Temp_Hum.id2          = id2;
  
  Send.Temp_Hum.temperatureh = temp >>8    ;
  if (temp>=0)
  	Send.Temp_Hum.tempsign     = 0   ;
  else
  	Send.Temp_Hum.tempsign     = 1   ;
  Send.Temp_Hum.temperaturel   = temp & 0xff;
  Send.Temp_Hum.battery_level  = 6;
  Send.Temp_Hum.rssi           = 9 ;
  Send.Temp_Hum.humidity       = hum ;
  Send.Temp_Hum.humidity_status= 0  ;
  
  
  Serial.write((byte*)&Send.Temp_Hum,sizeof(Send.Temp_Hum));
              
}
void reportDomoticTempHum ( const byte* data){
//	reportDomoticTempHum (temperatureint(data) , getHumidity(data) , 0x47 , 0x00 );
	reportDomoticTempHum (temperatureint(data) , getHumidity(data) , data[3] ,data[2] );
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
  Send.IRESPONSE.msg2    = 8  ;    //Firmware version
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

void reportHagerDomotic ( const byte* data, byte pos ){
  

		Send.LIGHTING2.packetlength=11 + 4 ; //+2 debug 
		Send.LIGHTING2.packettype = pTypeLighting2;   
		Send.LIGHTING2.subtype    = sTypeANSLUT ;							//AC
		Send.LIGHTING2.seqnbr			= 0 ;
		Send.LIGHTING2.id1        = data[0];            /* id emetteur 0..3  */
		Send.LIGHTING2.id2        = data[1];            /* id emetteur 0..FF */
		Send.LIGHTING2.id3        = data[2];            /* id emetteur 0..FF */
		Send.LIGHTING2.id4        = 0      ;            /* id emetteur 0..FF */
		Send.LIGHTING2.unitcode   = GetZone(data);  		/* unit = zone 1..3  */
		Send.LIGHTING2.level    =0 ;   /* dim level 0..15   */
		Send.LIGHTING2.rssi     = 0;
//devbug
		Send.LIGHTING2.data[0]  = data[3];   
		Send.LIGHTING2.data[1]  = data[4];
		Send.LIGHTING2.data[2]  = data[5];
		Send.LIGHTING2.data[3]  = data[6];


		if ( GetMode(data)== CONFOR)
				Send.LIGHTING2.cmnd       = 1 ;         
		else
				Send.LIGHTING2.cmnd       = 0 ;         
  Serial.write((byte*)&Send.LIGHTING2,Send.LIGHTING2.packetlength+1  );
}
