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
        count = SIZE_CODE;
        return Code;
    }
    const byte* DecodeMD230::getData () const {
        return Code;
    }
     byte DecodeMD230::getBytes ()  {
        return SIZE_CODE;
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
    bool DecodeMD230::nextOnePulse (word pWidth , byte pdata)   {
        //pulse lsb wil be 10 micros to save computation time
        byte Swidth = pWidth/WIDTH_DIVIDER ;

        switch(state)
        {
        case UNKNOWN :  /* test reception pulse high */
                        if ( pWidth>PULSE_SYNCHRO_LOW) 
                          state = T0; 

            break;   
        case T0      :  /* test reception pulse high synchro */
											if ( TEST_PULSE_WINDOW(Swidth,PULSE_SYNCHRO_HIGH_MIN,PULSE_SYNCHRO_HIGH_MAX,WIDTH_DIVIDER) )
														state = T1 ;/*  synchro receive */
                       else
                           resetDecoder ();
            break;   
        case T1      : //reception data bit pulse low
            if      ( TEST_PULSE_WINDOW(Swidth,PULSE_ONE_LOW_MIN,PULSE_ONE_LOW_MAX,WIDTH_DIVIDER)  ){
                lastBit=1;
                state = T2 ;
            }
            else if ( TEST_PULSE_WINDOW(Swidth,PULSE_ZERO_LOW_MIN,PULSE_ZERO_LOW_MAX,WIDTH_DIVIDER)  ){
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
            if ( TEST_PULSE_WINDOW(Swidth,PULSE_ONE_HIGH_MIN,PULSE_ONE_HIGH_MAX,WIDTH_DIVIDER)  )
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
            else  if ( TEST_PULSE_WINDOW(Swidth,PULSE_ZERO_HIGH_MIN,PULSE_ZERO_HIGH_MAX,WIDTH_DIVIDER)  )
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
    Serial.print (Code[0],HEX);Serial.print(' ');
    Serial.print (Code[1],HEX);Serial.print(' ');
    Serial.print (Code[2],HEX);Serial.print(' ');
    Serial.print (Code[3],HEX);Serial.print(' ');
    Serial.print (Code[4],HEX);Serial.print(' ');
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


