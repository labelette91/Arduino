#pragma once

#include  "reportSerial.h"

#ifdef WIN32
#else
#endif    

#define REPORT_DOMOTIC 0
#define REPORT_SERIAL       1
#define SERIAL_DEBUG 2


#define INVALID_INT 0x7F

typedef enum {
	LSB_FIRST = 0,
	MSB_FIRST = 1,
} TBIT_STREAM ;

class DecodeOOK {
public:
    byte total_bits, max_bits,bits, flip, state, pos, data[25];

	TBIT_STREAM bitStream = MSB_FIRST;
    byte lastId ;
    byte lastChannel ;
    byte lastCrc    ;

	
	virtual char decode(word width, byte level) { return -1; };

    virtual float getTemperature() {	  return (INVALID_INT);  }
    virtual byte  gethumidity()  {	  return (INVALID_INT);  }
    virtual byte getId()         {	  return (INVALID_INT);  }
    virtual byte getCrc()         {	  return (INVALID_INT);  }
    
    virtual byte getBatteryLevel() {		  return 15;  } //return 15 if batterie OK  
    virtual byte getChannel()      {	  return  1;  }    
    virtual float getPressure()      {	  return  INVALID_INT;  }    
    virtual char* getName()      {	  return  "";  }    
    virtual bool isValid()      		 {	  return  true;  }    
    virtual bool  newPacket()       
    {	 
        if ( lastId != getId()  || lastChannel != getChannel() || lastCrc != getCrc()  )
        {
            lastId      = getId() ;
            lastChannel =  getChannel() ;
            lastCrc     = getCrc();
            return  true ;  
        }
            
        return  false  ;  
    }

    virtual  void ReportSerial(byte rtype) 
	{
        Serial.print(getName());
		Serial.print(' ');
        Serial.print(millis() / 1000);
		int temp = (int)getTemperature();
		if (temp != INVALID_INT )
		{
			Serial.print(" T:");
			Serial.print(temp );
		}
        Serial.print(" Id:");
        Serial.print(getId() );
        Serial.print(" Chn:");
        Serial.print(getChannel());
        Serial.print(" Bat:");
        Serial.print(getBatteryLevel());
		byte hum = gethumidity() ;
		if (hum != INVALID_INT )
		{
			Serial.print(" Hum:");
			Serial.print(hum);
		}
		float pressure = getPressure() ;
		if (pressure != INVALID_INT )
		{
			Serial.print(" Pressure:");
			Serial.print(pressure);
		}
		
		if (rtype == SERIAL_DEBUG ){
			Serial.print(' ');
			printBinary (data, pos , 8 );
		}
        Serial.print('\n');
        Serial.print('\r');
    }
	virtual void report(byte rtype)
	{
		if (rtype == REPORT_DOMOTIC ){
//				if (gethumidity() != INVALID_INT)
//					  reportDomoticTempHum (getTemperature(), gethumidity(), getId(), getChannel(), getBatteryLevel());
		}
		else	
		{
					  ReportSerial(rtype);
		}    
	}
public:

    enum { UNKNOWN, T0, T1, T2, T3, OK, DONE };

    DecodeOOK () { resetDecoder(); }

	virtual bool nextPulse(word width, byte level=0) {
        if (state != DONE)
        
			switch (decode(width, level)) {
                case -1: resetDecoder(); break;
                case 1:  done(); break;
            }
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
        max_bits = 160;
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
};



