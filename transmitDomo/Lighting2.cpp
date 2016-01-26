#if _MSC_VER 
#include "debug\vspde.h"
#endif

#include "arduino.h"
#include "lighting2.h"

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

