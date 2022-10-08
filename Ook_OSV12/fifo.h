#ifndef _Fifo
#define _Fifo
#include "Arduino.h"

typedef word TBufferType  ;
typedef byte TBufferPtrType  ;

#define SIZE_FIFO 128

class TFifo {

public:
void init () ;
void put(TBufferType p);
TBufferType get();
bool empty();	
TFifo();
~TFifo();
void clear();

TBufferType    Pulse[SIZE_FIFO];
TBufferPtrType PWr;
TBufferPtrType PRd;
	
};

#endif