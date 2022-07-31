#include "Fifo.h"

TFifo::TFifo()
{
	init();
}
TFifo::	~TFifo()
{
}

void TFifo::init()
{
PWr=0;
PRd=0;
}
bool TFifo::empty()
{
return(PWr==PRd);
}
void TFifo::put(TBufferType p)
{
    Pulse[PWr++]=p;
    if (PWr >=  SIZE_FIFO ) PWr=0;
}
TBufferType TFifo::get()
{
	TBufferType p=0 ;
	if (PWr!=PRd)
	{
    p=Pulse[PRd++];
    if (PRd >=  SIZE_FIFO ) PRd=0;
  }
  return p;
}

void TFifo::clear()
{
	PRd = PWr;
}