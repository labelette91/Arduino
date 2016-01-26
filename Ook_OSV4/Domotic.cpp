#if _MSC_VER 
#include "debug\vspde.h"
#endif

#include "arduino.h"
#include "Domotic.h"

#include "Oregon.h"

tRBUF Cmd ;
unsigned long  lastTime;  


unsigned long getLightingId (){
	                return   (( unsigned long  )Cmd.LIGHTING2.id1) << 24 +
                          (( unsigned long  )Cmd.LIGHTING2.id2) << 16 +
                          (( unsigned long  )Cmd.LIGHTING2.id3) <<  8 +
                          (( unsigned long  )Cmd.LIGHTING2.id4)       ;

}
void DomoticInit(){
lastTime =  0;
}
//reception d'une commande de send domotic
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
            Cmd.LIGHTING2.receiveLength = 0 ;
            /* verification packettype */
            if (Cmd.LIGHTING2.packettype==17) {
                return true;
            } else{
                return false;
            }
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
	reportDomoticTempHum (temperatureint(data) , getHumidity(data) , 0x47 , 0x00 );
}
