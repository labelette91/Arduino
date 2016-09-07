#ifndef _DecodeMD230_
#define _DecodeMD230_

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>            //assumes Arduino IDE v1.0 or greater
#else
#include "deftype.h"
#endif

#define NBBIT  36
#define PULSE_HIGH 400
#define PULSE_ONE  1300
#define PULSE_ZERO 300

#define PULSE_SYNCHRO_LOW  3840
#define PULSE_SYNCHRO_HIGH 330 

#define TOLERANCE 200

#define MD230_ID0 0x3C
#define MD230_ID1 0x80

//#define _debug

/*  36 bit */
/* begin with low synchro pulse of  PULSE_SYNCHRO followed by high pulse 330탎
/* bit 1 : pulse 0 250 탎 pulse 1 750 탎 */
#define PULSE_ONE_LOW  250
#define PULSE_ONE_HIGH 750

/* bit 0 : pulse 0 660 탎 pulse 1 330 탎 */
#define PULSE_ZERO_LOW  660
#define PULSE_ZERO_HIGH 330


#define SIZE_CODE 5 

#define TEST_PULSE(WIDTH,PULSE_LEN,TOL)((WIDTH> (PULSE_LEN-TOL))&&(WIDTH<(PULSE_LEN+TOL)))

#define COPY_CODE(dest,src) dest[0]=src[0];\
                            dest[1]=src[1];\
                            dest[2]=src[2];\
                            dest[3]=src[3];\
                            dest[4]=src[4];\

#define TEST_CODE(dest,src) ( ( dest[0]==src[0]) &&\
                              ( dest[1]==src[1]) &&\
                              ( dest[2]==src[2]) &&\
                              ( dest[3]==src[3]) &&\
                              ( dest[4]==src[4])   \
                             )

#define RESET_CODE(dest)    dest[0]=0;\
                            dest[1]=0;\
                            dest[2]=0;\
                            dest[3]=0;\
                            dest[4]=0;\


class DecodeMD230 {
public:
    byte total_bits, state, MaxCode , NbCodeOk , lastBit , pos, data[10]; ;
    byte CurCode[SIZE_CODE] , Code[SIZE_CODE] , tmpCode[SIZE_CODE], lastCode[SIZE_CODE];
    unsigned long LastReceived ; 

public:
    enum { UNKNOWN, T0, T1, T2, T3, OK, DONE };

    const byte* getData (byte& count) const ;
    const byte* getData () const ;
     byte getBytes ()  ;

    void resetDecoder () ;
    
    DecodeMD230 (byte pMaxCode=5) ;
    bool nextOnePulse (word pWidth , byte data)   ;
    
  bool nextPulse (word width , byte BitData)   ;
  void ReportSerial();

 };

#endif