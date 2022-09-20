#ifndef _DecodeRain_
#define _DecodeRain_

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>            //assumes Arduino IDE v1.0 or greater
#else
#include "deftype.h"
#endif

#include "DecodePwm.h"


#define TEST_PULSE(WIDTH,PULSE_LEN,TOL)((WIDTH> (PULSE_LEN-TOL))&&(WIDTH<(PULSE_LEN+TOL)))

class DecodeRain  : public DecodePwm{
//class DecodeRain {
public:
public:
    
    DecodeRain (byte pMaxCode=2)  ;
    virtual void report();
    virtual bool isValid();

 };

#endif