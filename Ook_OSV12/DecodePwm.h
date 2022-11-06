#ifndef _DecodePwm_
#define _DecodePwm_

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>            //assumes Arduino IDE v1.0 or greater
#else
#include "deftype.h"
#endif

#include "DecodeOOK.h"


class DecodePwm  : public DecodeOOK{
//class DecodePwm {
public:
    byte  PulseHigh1 ,  PulseLow1  , PulseHigh0 , PulseLow0  ;
    byte  SynchroHigh;
    byte  SynchroLow; 
    byte  Tolerance ;
public:
    enum { UNKNOWN, T0, T1, T2, T3, OK, DONE , WAITSyncHigh,  WAITSyncLow , WAITBitHigh , WAITBit0High  , WAITBit0Low , WAITBit1High , WAITBit1Low};

    void resetDecoder () ;
    
    DecodePwm (byte pMaxCode=2)  ;
    void SetPulseDuration ( word pPulseHigh1, word pPulseLow1, word pPulseHigh0, word pPulseLow0,  word  pSynchroHigh, word  pSynchroLow, word pTolerance )    ;
  sbyte decode (word width , byte BitData)   ;
 };

#endif