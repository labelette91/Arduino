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
          lastBit = 0xFF ;
    }    
    //pMaxCode : the number of successive received equal value to return the current Code
    
    DecodeHomeEasy::DecodeHomeEasy (byte pMaxCode) { 
        resetDecoder(); 
        PacketCountSeuil=pMaxCode;
        Name ="H";
    }


    /* T0 : attente bit de synchro */

    /* result in CurCode */
    sbyte DecodeHomeEasy::decode (word pWidth , byte data)   {
        //pulse lsb wil be 100 micros to save computation time
//        byte width = pWidth / 100;
//        byte width = pWidth ;
        word width = pWidth ;

        switch(state)
        {
        case UNKNOWN :  /* test reception pulse high */
                        //if ( TEST_PULSE(width,PULSE_HIGH,TOLERANCE)  && (data==1) )
        case T0      :  /* test reception pulse low synchro */
						if ( TEST_PULSE(width,PULSE_SYNCHRO_LONG,1000)  && (data==0) )
							state = T1 ;/*  synchro receive */
//                       else
//                           resetDecoder ();
            break;   
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
                        gotBit(0);
                        state = T1 ;
#ifdef _debug
                        printf ( "Bits : %02d %d %08X\n", total_bits , lastBit , CurCode ) ;
#endif
                    }
                    else
                        resetDecoder ();
                }
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
                        gotBit(1);
                        state = T1 ;
#ifdef _debug
                        printf ( "Bits : %02d %d %08X \n", total_bits , lastBit, CurCode ) ;
#endif
                    }
                    else
                        resetDecoder ();
                }
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
            return true;
        }
        return false;   
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



