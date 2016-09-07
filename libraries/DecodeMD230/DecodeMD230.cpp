#include "DecodeMD230.h"
#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>            //assumes Arduino IDE v1.0 or greater
#else
#include <stdio.h>
#include <wiringPi.h>
//#include <wiringPiSPI.h>
#include <print.h>

#endif
/* bit 1 : |-|____ : short Pulse One / long  Pulse Low  */
/* bit 0 : |-|_    : short Pulse One / short Pulse Low  */


const byte* DecodeMD230::getData (byte& count) const {
        count = pos;
        return data; 
    }
    const byte* DecodeMD230::getData () const {
        return data; 
    }
     byte DecodeMD230::getBytes ()  {
        return pos ; 
    }

    void DecodeMD230::resetDecoder () {
          total_bits =  0;
          state = UNKNOWN;
          RESET_CODE(CurCode)  ;
          lastBit = 0xFF ;
          pos=0 ;
    }    
    //pMaxCode : the number of successive received equal value to return the current Code
    
    DecodeMD230::DecodeMD230 (byte pMaxCode) { lastBit = 0xFF ; RESET_CODE(Code) ; NbCodeOk=0;MaxCode=pMaxCode;RESET_CODE(tmpCode);RESET_CODE(lastCode);LastReceived=0;resetDecoder(); }


    /* T0 : attente bit de synchro */

    /* result in CurCode */
    bool DecodeMD230::nextOnePulse (word pWidth , byte data)   {
        //pulse lsb wil be 100 micros to save computation time
//        byte width = pWidth / 100;
//        byte width = pWidth ;
        word width = pWidth ;

        switch(state)
        {
        case UNKNOWN :  /* test reception pulse high */
                        if ( width>PULSE_SYNCHRO_LOW) 
                          state = T0; 

            break;   
        case T0      :  /* test reception pulse high synchro */
											if ( TEST_PULSE_WINDOW(width,PULSE_SYNCHRO_HIGH_MIN,PULSE_SYNCHRO_HIGH_HIGH) )
														state = T1 ;/*  synchro receive */
                       else
                           resetDecoder ();
            break;   
        case T1      : //reception data bit pulse low
            if      ( TEST_PULSE_WINDOW(width,PULSE_ONE_LOW_MIN,PULSE_ONE_LOW_MAX)  ){
                lastBit=1;
                state = T2 ;
            }
            else if ( TEST_PULSE_WINDOW(width,PULSE_ZERO_LOW_MIN,PULSE_ZERO_LOW_MAX)  ){
                lastBit=0;
                state = T2 ;
            }
            else
                resetDecoder ();
            break;   
        case T2      : //reception dataBit etat high
            state = T1 ;
            pos =total_bits/8 ;
            total_bits++;
            if ( TEST_PULSE_WINDOW(width,PULSE_ONE_HIGH_MIN,PULSE_ONE_HIGH_MAX)  )
            {
                    //the previus bit shall be 1 
                    if (lastBit==1)
                    {
                        /* add bit 1 to Code */
                        CurCode[pos] <<= 1;
                        CurCode[pos] |=  1 ;
                    }
                    else
                        resetDecoder ();
            }
            else  if ( TEST_PULSE_WINDOW(width,PULSE_ZERO_HIGH_MIN,PULSE_ZERO_HIGH_MAX)  )
            {
                    //the previus bit shall be 0
                    if (lastBit==0)
                    {
                        /* add bit 0 to Code */
                        CurCode[pos] <<= 1;
                    }
                    else
                        resetDecoder ();
            }
            else
               resetDecoder ();
#ifdef _debug
            printf ( "Bits : %02d %d %08X\n", total_bits , lastBit , CurCode ) ;
#endif
            break;   
        }

        if (total_bits==NBBIT)
        {
            //update code
            done();
            COPY_CODE(tmpCode , CurCode );
            resetDecoder ();
            return true;
        }
        return false;   
    }

    /* return true if successive equal MaxCode 24 bits words have been receive between two 8ms pulse */
    /* result in Code */
    
  bool DecodeMD230::nextPulse (word width , byte BitData)   {
    //if one frame has been received
    if (nextOnePulse ( width ,  BitData) ){
       if (NbCodeOk==0) COPY_CODE(lastCode , tmpCode) ;
       //if same value increment counter or decrement
       if ( TEST_CODE(lastCode,tmpCode) ) NbCodeOk ++ ; else NbCodeOk -- ;
       //if max number of same frame has been received
       if (NbCodeOk >= MaxCode ){
          NbCodeOk = 0 ;
          if ( !( TEST_CODE(Code,lastCode) ) ) {
            //set final value
            COPY_CODE(Code , lastCode );
            LastReceived = millis();
            data[0] = MD230_ID0 ;
            data[1] = MD230_ID1 ;
            //unit code 
            data[2]  = (byte)(lastCode [4] ); 
            //switch value 
            data[3 ] = (byte)(lastCode [4]) ; 
            //group value 
            data[4 ] = (byte)(lastCode [4]) ; 
            //id 3FFFFFF
            data[8]  = (byte)(lastCode[0] ) ;  
            data[7]  = (byte)(lastCode[1] ) ;  
            data[6]  = (byte)(lastCode[2] ) ;  
            data[5]  = (byte)(lastCode[3] ) ;  
            pos = 9;
            return true;
          }
          //report only if new value has not been report since 2 min
//          if ( (millis()-LastReceived) > 120000 )
          {
            LastReceived = millis();
            return true;
          }
       } 
    }
    return false ;
  }  
  

  
void DecodeMD230::ReportSerial(){
    Serial.print ("MD230:");
    Serial.print (Code[0],HEX);
    Serial.print (Code[1],HEX);
    Serial.print (Code[2],HEX);
    Serial.print (Code[3],HEX);
    Serial.print (Code[4],HEX);
    Serial.print ('\n');
    Serial.print ('\r');

  }

void DecodeMD230::done () {
        while (total_bits%8)
            gotBit(0); // padding
        state = DONE;
    }

// add one bit to the packet data buffer

void DecodeMD230::gotBit (byte value) {
    pos =total_bits/8 ;
    total_bits++;
    CurCode[pos] <<= 1;
    CurCode[pos] |=  value ;
}


