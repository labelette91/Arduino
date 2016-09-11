#ifndef _DecodeMD230_
#define _DecodeMD230_

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>            //assumes Arduino IDE v1.0 or greater
#else
#include "deftype.h"
#endif

#define NBBIT  36

#define PULSE_SYNCHRO_LOW  1000
#define PULSE_SYNCHRO_HIGH_MIN 140 
#define PULSE_SYNCHRO_HIGH_MAX 390 

#define TOLERANCE 400

#define MD230_ID0 0x3C
#define MD230_ID1 0x80

//#define _debug

/*  36 bit */
/* begin with low synchro pulse of  PULSE_SYNCHRO followed by high pulse 330탎
/* bit 1 : pulse 0  750..950 850+-100 탎   탎 pulse 1 220..400 = 310+-90*/
//#define PULSE_ONE_LOW  850
#define PULSE_ONE_LOW_MIN  750
#define PULSE_ONE_LOW_MAX  950

//#define PULSE_ONE_HIGH 310
#define PULSE_ONE_HIGH_MIN 150
#define PULSE_ONE_HIGH_MAX 400

/* bit 0 : pulse 0 380..640=510+-130    탎 pulse 1 590..760 s */
//#define PULSE_ZERO_LOW  510
#define PULSE_ZERO_LOW_MIN  380
#define PULSE_ZERO_LOW_MAX  640


//#define PULSE_ZERO_HIGH 330
#define PULSE_ZERO_HIGH_MIN 590
#define PULSE_ZERO_HIGH_MAX 760


#define SIZE_CODE 5 

//valeur for the division of pulse width
#define WIDTH_DIVIDER 10

#define TEST_PULSE_TOL(WIDTH,PULSE_LEN,TOL)((WIDTH>= (PULSE_LEN-TOL))&&(WIDTH<=(PULSE_LEN+TOL)))

#define TEST_PULSE_WINDOW(WIDTH,PULSE_MIN,PULSE_MAX,DIVIDER)((WIDTH>= (PULSE_MIN/DIVIDER))&&(WIDTH<=(PULSE_MAX/DIVIDER)))


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
    byte total_bits, state, MaxCode , NbCodeOk , lastBit , pos ;
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
  void done ();
 virtual void gotBit (byte value) ;
 };

#endif