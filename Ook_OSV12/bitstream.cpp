
#include <stdint.h>
#include "bitstream.h"

#define GETRAWVALUE(RETURN_TYPE,SIZE_WORD_IN_BIT)                                                                          \
    if (size > sizeof(RETURN_TYPE)*8 )                                                         \
		    return 0 ;                                                                         \
                                                                                               \
    RETURN_TYPE Value = 0 ;                                                                    \
    uint8_t srcByte    = (offset)   / SIZE_WORD_IN_BIT  ;                                      \
    uint8_t srcBitOfs  = SIZE_WORD_IN_BIT-1 - (offset)  % SIZE_WORD_IN_BIT ;                   \
    uint8_t availableBit  = srcBitOfs + 1 ;                                                    \
    while ( size != 0 )                                                                        \
    {                                                                                          \
        uint8_t val  = data[srcByte];                                                          \
        if (availableBit<8)                                                                    \
            val &=  (1<<availableBit)-1 ;                                                      \
        if (size>=SIZE_WORD_IN_BIT)                                                            \
            Value <<= SIZE_WORD_IN_BIT ;                                                       \
        else                                                                                   \
            Value <<= size ;                                                                   \
        if ( size >= availableBit ) {                                                          \
            Value |= val ;                                                                     \
            size -=  availableBit ;                                                            \
        }                                                                                      \
        else{                                                                                  \
            Value |= val >> (availableBit-size) ;                                              \
            size =  0  ;                                                                       \
        }                                                                                      \
        srcByte++;                                                                             \
        availableBit = SIZE_WORD_IN_BIT;                                                       \
    }                                                                                          \
    return Value;


uint32_t GetRaw1bValue(uint8_t * data , uint16_t offset, uint8_t size)
{
  uint32_t value =0;
if (size > 32)
		return 0 ;

uint16_t bitDest = offset+size-1;
for ( uint8_t bitsrc=0; bitsrc<size;bitsrc++)
{
    uint16_t destByte = bitDest /8 ;
    uint16_t DestBit  = 7 - bitDest %8 ;
    if(  data[destByte] & (1<<DestBit) ) 
        value |= (1L<<bitsrc);
    bitDest--;
}
return value;
}

uint32_t getRaw32bValue(uint8_t * data , uint8_t offset, uint8_t size)
{
    GETRAWVALUE(uint32_t,8);
}

uint16_t getRaw16bValue(uint8_t * data , uint8_t offset, uint8_t size)
{
    GETRAWVALUE(uint16_t,8);
}

uint8_t getRaw08bValue(uint8_t * data , uint8_t offset, uint8_t size)
{
    GETRAWVALUE(uint8_t,8);
}

#if 1 
#include "Arduino.h"
uint8_t Data[10];
void bitSreamtest()
{

    Data[0] = 0x55 ;
    Data[1] = 0xAA ;
    Data[2] = 0x55 ;
    Data[3] = 0xAA ;
    Data[4] = 0x55 ;
    Data[5] = 0xAA ;
    uint32_t d1,d2;
      Serial.print("test bitstream\n");
for ( int offs=0;offs<16;offs++)
        for ( int size=1;size<=32;size++)
            //             check(Data , offs,  size,0x2AAA);
//            if ( offs == 1 && size == 17 )
            if ((d1=getRaw32bValue(Data , offs, size)) != (d2=GetRaw1bValue(Data , offs, size)) ) 
            { Serial.print(" error1 " ); Serial.print( offs );Serial.print( ' ' );Serial.print( size ); Serial.print( ' ' );Serial.print( d1,HEX );Serial.print( ' ' );Serial.print( d2,HEX ); Serial.println();}

    for ( int offs=0;offs<16;offs++)
        for ( int size=1;size<=16;size++)
            if (getRaw16bValue(Data , offs, size) != GetRaw1bValue(Data , offs, size) ) 
            { Serial.print(" error2 " ); Serial.print( offs );Serial.print( ' ' );Serial.println( size ); }
    for ( int offs=0;offs<16;offs++)
        for ( int size=1;size<=8;size++)
            if (getRaw08bValue(Data , offs, size) != GetRaw1bValue(Data , offs, size) ) 
            { Serial.print(" error3 " ); Serial.print( offs );Serial.print( ' ' );Serial.println( size ); }

}
#endif

