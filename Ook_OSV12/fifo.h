#ifndef _Fifo
#define _Fifo
#include "arduino.h"

typedef word TBufferType  ;
typedef byte TBufferPtrType  ;

#define SIZE_FIFO 16

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