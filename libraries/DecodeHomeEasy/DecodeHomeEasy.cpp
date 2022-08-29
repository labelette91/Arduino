#include "DecodeHomeEasy.h"
#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>            //assumes Arduino IDE v1.0 or greater
#else
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

#endif

void reportDomoticHomeEasy(byte id1, byte id2, byte id3, byte id4, byte group, byte cmd, byte unitcode);
uint16_t getRaw16bValue(uint8_t* data, uint8_t offset, uint8_t size);
uint8_t getRaw08bValue(uint8_t* data, uint8_t offset, uint8_t size);

/* bit 1 : |-|____ : short Pulse One / long  Pulse Low  */
/* bit 0 : |-|_    : short Pulse One / short Pulse Low  */


#define TEST_PULSE(WIDTH,PULSE_LEN,TOL)((WIDTH> (PULSE_LEN-TOL))&&(WIDTH<(PULSE_LEN+TOL)))

const byte* DecodeHomeEasy::getData (byte& count) const {
        count = pos;
        return data; 
    }
    const byte* DecodeHomeEasy::getData () const {
        return data; 
    }
     byte DecodeHomeEasy::getBytes ()  {
        return pos ; 
    }

    void DecodeHomeEasy::resetDecoder () {
          DecodeOOK::resetDecoder ();
          total_bit =  0;
          state = UNKNOWN;
          CurCode = 0 ;
          lastBit = 0xFF ;
          pos=0 ;
    }    
    //pMaxCode : the number of successive received equal value to return the current Code
    
    DecodeHomeEasy::DecodeHomeEasy (byte pMaxCode) { lastBit = 0xFF ; Code = 0 ; NbCodeOk=0;MaxCode=pMaxCode;tmpCode=0;lastCode=0;LastReceived=0;resetDecoder(); }


    /* T0 : attente bit de synchro */

    /* result in CurCode */
    char DecodeHomeEasy::decode (word pWidth , byte data)   {
        //pulse lsb wil be 100 micros to save computation time
//        byte width = pWidth / 100;
//        byte width = pWidth ;
        word width = pWidth ;

        switch(state)
        {
        case UNKNOWN :  /* test reception pulse high */
                        //if ( TEST_PULSE(width,PULSE_HIGH,TOLERANCE)  && (data==1) )
        case T0      :  /* test reception pulse low synchro */
//						if (width> PULSE_SYNCHRO) 
//							state = T1 ;/*  synchro receive */
//                       else
//                           resetDecoder ();
//            break;   
        case T1      : //reception data bit pulse high
            if ( TEST_PULSE(width,PULSE_HIGH,TOLERANCE)  )
                state = T2 ;
            else
                resetDecoder ();
            break;   
        case T2      : //reception dataBit etat low
            /* test long pulse */
            if ( TEST_PULSE(width,PULSE_ONE,TOLERANCE)  )
            {
                state = T1 ;
                total_bit++;
                if ((total_bit%2)==0)
                {
                    //the previus bit shall be 0 bit 0 
                    if (lastBit==0)
                    {
                        /* add bit 0 to Code */
                        CurCode = CurCode << 1;
                        gotBit(0);
#ifdef _debug
                        printf ( "Bits : %02d %d %08X\n", total_bits , lastBit , CurCode ) ;
#endif
                    }
                    else
                        resetDecoder ();
                }
                state = T1 ;
                lastBit = 1 ;
            }
            /* test short pulse */
            else  if ( TEST_PULSE(width,PULSE_ZERO,TOLERANCE)  )
            {
                state = T1 ;
                total_bit++;
                if ((total_bit%2)==0)
                {
                    //the previus bit shall be 1
                    if (lastBit==1)
                    {
                        /* add bit 1 to Code */
                        CurCode = CurCode << 1;
                        CurCode = CurCode | 1 ;
                        gotBit(1);
#ifdef _debug
                        printf ( "Bits : %02d %d %08X \n", total_bits , lastBit, CurCode ) ;
#endif
                    }
                    else
                        resetDecoder ();
                }
                state = T1 ;
                lastBit = 0 ;
            }
            /* test synchro pulse */
            else  if (  (width> PULSE_SYNCHRO) )
            {

               state = T1 ;/*  synchro receive */
            }
            else
               resetDecoder ();
            break;   
        }

        if (total_bit==NBBIT*2)
        {
            //update code
            tmpCode = CurCode ;
//            resetDecoder ();
            return true;
        }
        return false;   
    }

    /* return true if successive equal MaxCode 24 bits words have been receive between two 8ms pulse */
    /* result in Code */
    
  bool DecodeHomeEasy::nextOnePulse (word width , byte BitData)   {
    //if one frame has been received
    if (nextPulse ( width ,  BitData) ){
       if (NbCodeOk==0) lastCode = tmpCode ;
       //if same value increment counter or decrement
       if (lastCode == tmpCode ) NbCodeOk ++ ; else NbCodeOk -- ;
       //if max number of same frame has been received
       if (NbCodeOk >= MaxCode ){
          NbCodeOk = 0 ;
          if ( Code != lastCode ) {
            //set final value
            Code = lastCode ;
            LastReceived = millis();
            data[0] = HOMESWITCH_ID0 ;
            data[1] = HOMESWITCH_ID1 ;
            //unit code 
            data[2]  = (byte) (lastCode & 0x0000000F ); 
            lastCode = lastCode >> 4 ;
            //switch value 
            data[3 ] = (byte)(lastCode & 0x00000001) ; 
            lastCode = lastCode >> 1 ;
            //group value 
            data[4 ] = (byte)(lastCode & 0x00000001) ; 
            lastCode = lastCode >> 1 ;
            //id 3FFFFFF
            data[8]  = (byte)(lastCode & 0x000000FF) ;  lastCode = lastCode >> 8 ;
            data[7]  = (byte)(lastCode & 0x000000FF) ;  lastCode = lastCode >> 8 ;
            data[6]  = (byte)(lastCode & 0x000000FF) ;  lastCode = lastCode >> 8 ;
            data[5]  = (byte)(lastCode & 0x00000003) ;  
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
  

  
  void DecodeHomeEasy::report(){

        byte id1 = getRaw08bValue((uint8_t*)data,0,2);
        byte id2 = getRaw08bValue((uint8_t*)data,2,8);
        byte id3 = getRaw08bValue((uint8_t*)data,10,8);
        byte id4 = getRaw08bValue((uint8_t*)data,18,8);
        byte group = getRaw08bValue((uint8_t*)data,26,1);
        byte cmd   = getRaw08bValue((uint8_t*)data,27,1);
        byte unitcode  = getRaw08bValue((uint8_t*)data,28,4);

      reportDomoticHomeEasy(  id1   , id2   , id3   , id4   , group , cmd   , unitcode);
  }



