#pragma once

#include <string.h>
//#include  "reportSerial.h"
void printHexa ( byte * data, byte pos);

#ifdef WIN32
#else
#endif    

#define sbyte signed char 

void reportPrint(const char * mes);

//#define DEBUG_STATE 
//#define DEBUG_COUNT 

static const char* Sstate[] = {
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



typedef enum {
	LSB_FIRST = 0,
	MSB_FIRST = 1,
} TBIT_STREAM ;

class DecodeOOK {
public:
    byte total_bits, max_bits,bits, flip, state, pos, data[16];

	TBIT_STREAM bitStream = MSB_FIRST;
//    byte lastId ;
//    byte lastChannel ;
    byte lastCrc    ;

	byte lastdata[4] ;
	byte PacketCount ;
    byte PacketCountSeuil = 1 ; //nombre de packets identique recu pout detecter un nouveau packet 
    unsigned long LastSend ;
    const char* Name ="U";
	
	virtual sbyte decode(word width, byte level) { return -1; };

//    virtual float getTemperature() {	  return (INVALID_TEMP);  }
//    virtual byte  gethumidity()  {	  return (INVALID_HUM);  }
//    virtual byte getId()         {	  return (INVALID_BYTE);  }
    virtual byte getCrc()         {	  return (0);  }
//    virtual byte getBatteryLevel() {		  return 15;  } //return 15 if batterie OK  
//    virtual byte getChannel()      {	  return  1;  }    
//    virtual float getPressure()      {	  return  INVALID_PRESSURE;  }    
    virtual const char* getName()      {	  return  Name;  }    
    virtual bool isValid()      		 {	  return  true;  }    
    virtual bool  newPacket()       
    {	 

        if (!isValid())
        {
            reportPrint ("BADCHK ");          //ReportSerial();
            reportPrint (Name);          //ReportSerial();
            printHexa(data, pos );

            reportPrint ("\n");          //ReportSerial();
            resetDecoder();
            return  false  ;  
        }
        //send at least every 2min
        if ((millis() - LastSend) > 120000)  { lastdata[0]=lastdata[1]=lastdata[2]=lastdata[3] = 0;LastSend = millis();}
        
        countPacket();

#ifdef DEBUG_COUNT
         Serial.print('C');Serial.print(getName());    Serial.print(PacketCount);   Serial.print(' ');
#endif
        if (PacketCount == PacketCountSeuil) {
            LastSend = millis();
#ifdef DEBUG_COUNT
            Serial.println();
#endif
            return true;
        }

        return  false  ;  

    }
    virtual void report()=0;

public:

    enum { UNKNOWN, T0, T1, T2, T3, OK, DONE };

    DecodeOOK () { max_bits=160; memset(data,0,sizeof(data)); resetDecoder(); }

	virtual bool nextPulse(word width, byte level=0) {
        if (state != DONE)
        
			switch (decode(width, level)) {
                case -1: resetDecoder(); break;
                case 1:  done(); break;
            }
#ifdef  DEBUG_STATE 
        {
            printf( "%c = (%4d,%d)  %2d %2d :%s\n", Name[0],width,level, state , total_bits , Sstate[state]); 
        }

#endif
        return isDone();
    }
    
    bool isDone () const { return state == DONE; }

    const byte* getData (byte& count) const {
        count = pos;
        return data; 
    }
    const byte* getData () const {
        return data; 
    }
    
    void resetDecoder () {
        total_bits = bits = pos = flip = 0;
        state = UNKNOWN;
    }
    
    // add one bit to the packet data buffer
    
    void gotBitMsb (char value) {
        total_bits++;
        byte *ptr = data + pos;
		*ptr = (*ptr << 1) | (value);

        if (++bits >= 8) {
            bits = 0;
            if (++pos >= sizeof data) {
                resetDecoder();
                return;
            }
        }
        state = OK;
    }
	 void gotBitLsb (char value) {
        total_bits++;
        byte *ptr = data + pos;
        *ptr = (*ptr >> 1) | (value << 7);

        if (++bits >= 8) {
            bits = 0;
            if (++pos >= sizeof data) {
                resetDecoder();
                return;
            }
        }
        state = OK;
    }
	virtual void gotBit (char value) 
	{
		if ( bitStream == MSB_FIRST )
			gotBitMsb ( value) ;
		else
			gotBitLsb ( value) ;
	
	}
    
    // store a bit using Manchester encoding
    void manchester (char value) {
        flip ^= value; // manchester code, long pulse flips the bit
        gotBit(flip);
    }
    
    // move bits to the front so that all the bits are aligned to the end
    void alignTail (byte max =0) {
        // align bits
        if (bits != 0) {
            data[pos] >>= 8 - bits;
            for (byte i = 0; i < pos; ++i)
                data[i] = (data[i] >> bits) | (data[i+1] << (8 - bits));
            bits = 0;
        }
        // optionally shift bytes down if there are too many of 'em
        if (max > 0 && pos > max) {
            byte n = pos - max;
            pos = max;
            for (byte i = 0; i < pos; ++i)
                data[i] = data[i+n];
        }
    }
    
    void reverseBits () {
        for (byte i = 0; i < pos; ++i) {
            byte b = data[i];
            for (byte j = 0; j < 8; ++j) {
                data[i] = (data[i] << 1) | (b & 1);
                b >>= 1;
            }
        }
    }
    
    void reverseNibbles () {
        for (byte i = 0; i < pos; ++i)
            data[i] = (data[i] << 4) | (data[i] >> 4);
    }
    
    void done () {
        while (bits)
            gotBit(0); // padding
        state = DONE;
    }

    byte countPacket()
    {
        if (
               (lastCrc     != getCrc()) 
            || (lastdata[0] != data[0])
            || (lastdata[1] != data[1])
            || (lastdata[2] != data[2])
            || (lastdata[3] != data[3])
            )
        {
            lastdata[0] = data[0];
            lastdata[1] = data[1];
            lastdata[2] = data[2];
            lastdata[3] = data[3];
            lastCrc     = getCrc();
            PacketCount = 0;
        }
        PacketCount++;
        return PacketCount;
    }

};



