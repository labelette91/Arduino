#include <Arduino.h>            //assumes Arduino IDE v1.0 or greater

#include "DecodePwm.h"

#define TO10(duree)(duree/10)

#define TEST_PULSE(WIDTH,PULSE_LEN,TOL)((WIDTH>= (PULSE_LEN-TOL))&&(WIDTH<=(PULSE_LEN+TOL)))

#ifdef _debugPwm_
const char* Sstate[] = {
"UNKNOWN", 
"T0", 
"T1", 
"T2", 
"T3", 
"OK", 
"DONE ", 
"WAITSyncHigh",  
"WAITSyncLow ", 
"WAITBitHigh ", 
"WAITBit0High  ", 
"WAITBit0Low ", 
"WAITBit1High ", 
"WAITBit1Low"

};
#endif

    void DecodePwm::SetPulseDuration ( word pPulseHigh1, word pPulseLow1, word pPulseHigh0, word pPulseLow0,  word   pSynchroHigh, word  pSynchroLow, word pTolerance  )    
    {
         PulseHigh1 = TO10(pPulseHigh1 ); 
         PulseLow1  = TO10(pPulseLow1  ); 
         PulseHigh0 = TO10(pPulseHigh0 ); 
         PulseLow0  = TO10(pPulseLow0  ); 
         SynchroHigh= TO10(pSynchroHigh) ;
         SynchroLow = TO10(pSynchroLow ) ; 
         Tolerance  = TO10(pTolerance ) ; 

    }
    void DecodePwm::resetDecoder () {
          DecodeOOK::resetDecoder ();
    }    
    //pMaxCode : the number of successive received equal value to return the current Code
    
    DecodePwm::DecodePwm (byte pMaxCode) { 
        resetDecoder(); 
        PacketCountSeuil=pMaxCode;
        Name ="P";
        max_bits = 17*4;

        SetPulseDuration(500,250,250,500,750,750, 100 );
    }

    /* result in CurCode */
    sbyte DecodePwm::decode (word pWidth , byte data)   {
        //pulse lsb wil be 10 micros to save computation time
        byte width = TO10(pWidth) ;

        switch(state)
        {
        case UNKNOWN :  /* test reception pulse high */
                        if ( TEST_PULSE(width,SynchroHigh,Tolerance)  && (data==1) )
                            state = WAITSyncLow ;
                        break ;
        case WAITSyncLow :  /* test reception pulse syn low */
                        if ( TEST_PULSE(width,SynchroLow,Tolerance)  && (data==0) )
                            state = WAITBitHigh ;
                        else
                            state = UNKNOWN ;
                        break ;

        case WAITBitHigh      :  
						if ( TEST_PULSE(width,PulseHigh1,Tolerance)  && (data==1) )
							state = WAITBit1Low ;
                        else
						if ( TEST_PULSE(width,PulseHigh0,Tolerance)  && (data==1) )
							state = WAITBit0Low ;
                        else
                        if ( TEST_PULSE(width,SynchroHigh,Tolerance)  && (data==1) ){
                            resetDecoder ();
                            state = WAITSyncLow ;
                        }
                        else
                        if ( TEST_PULSE(width,SynchroLow,Tolerance)  && (data==0) )
                        {
                            resetDecoder ();
                            state = WAITBitHigh ;
                        }
                        else
                            resetDecoder ();
                        break;   

        case WAITBit1Low      :  
						if ( TEST_PULSE(width,PulseLow1,Tolerance)  && (data==0) ){
                            gotBit(1);
							state = WAITBitHigh ;
                        }
                        else
                           resetDecoder ();
                        break;   
        case WAITBit0Low      :  
						if ( TEST_PULSE(width,PulseLow0,Tolerance)  && (data==0) ){
                            gotBit(0);
							state = WAITBitHigh ;
                        }
                        else
                           resetDecoder ();
                        break;   
        }
#ifdef _debugPwm_
        printf( "%2d %2d :%s\n",state , total_bits , Sstate[state]); 
#endif

        if (total_bits==max_bits)
        {
            return true;
        }
        return false;   
    }

 



