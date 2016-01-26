#if _MSC_VER 

#include "VSPDE.h"
#endif

// Oregon V2 decoder added - Dominique Pierre
// Oregon V3 decoder revisited - Dominique Pierre
// New code to decode OOK signals from weather sensors, etc.
// 2010-04-11 <jcw@equi4.com> http://opensource.org/licenses/mit-license.php
// $Id: ookDecoder.pde 5331 2010-04-17 10:45:17Z jcw $




class DecodeOOK {
public:
    byte total_bits, bits, flip, state, pos, data[25];

    virtual char decode (word width) =0;
    
public:

    enum { UNKNOWN, T0, T1, T2, T3, OK, DONE };

    DecodeOOK () { resetDecoder(); }

    bool nextPulse (word width) {
        if (state != DONE)
        
            switch (decode(width)) {
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
    }
    
    // add one bit to the packet data buffer
    
    virtual void gotBit (char value) {
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

#include "HagerDecoder.h"

HagerDecoder orscV2;
 
#define PORT 2

#define ledPin  13

//pin number for AIN1
#define AIN1 7

volatile word pulse;

long int 	LastReceive ;
int 		NbReceive;
word 		Dt;
long int 	NbPulse  ;


//si definit     : le signal entre sur int ext    d3 : int1
//si pas definit : le signal entre sur Analog ext d7 : AIN1

#define INT_EXTERNE 
#define DOMOTIC 1

#ifdef  INT_EXTERNE)
void ext_int_1(void) {
#else
ISR(ANALOG_COMP_vect) {
#endif
    static unsigned long  last;
    // determine the pulse length in microseconds, for either polarity
    pulse = micros() - last;
//    last += pulse;
    last = micros() ;
}

void reportSerial( char* s, class DecodeOOK& decoder) {
    byte pos;
    const byte* data = decoder.getData(pos);
    Serial.print(s);
    Serial.print(' ');
    Serial.print(" Np:");
    Serial.print(NbPulse);
    Serial.print(' ');



    for (byte i = 0; i < pos; ++i) {
        Serial.print(data[i] >> 4, HEX);
        Serial.print(data[i] & 0x0F, HEX);
        if (i==3) Serial.print(' ');
    }
    
    Serial.println();
}


void setup () {
	  LastReceive = 0 ;
	  NbReceive   = 0;
    
    Serial.begin(38400);
//    Serial.println("\n[ookDecoder]");
    
   // initialize the LED pin as an output:
    pinMode(ledPin, OUTPUT);       
    
    pulse=0;
}

extern byte ConfZ1[]  ;
extern byte ConfZ2[]  ;
extern byte ConfZ3[]  ;
extern byte Z2Eco [];
extern byte Z2Conf[];
extern byte Z3Eco [];
extern byte Z3Conf[];


/*
0704FA00	02208B4E	03827D00	011045A7
0704FA00	02208851	03827D00	01104428
			
0704FA00	02209346	03827D00	011049A3
0704FA00	02209049	03827D00	01104824
			
0704FA00	0220A336	03827D00	0110519B
0704FA00	0220A039	03827D00	0110501C
			
0704FA00	0220CC0D	03827D00	01106606
0704FA00	0220D405	03827D00	01106A02
			
0704FA00	020501F3	03827D00	010280F9
0704FA00	020502F2	03827D00	01028179
0704FA00	020504F0	03827D00	01028278

*/

byte i=0;
byte j=1;
byte * conf = ConfZ1;

void setPulse()
{

if ( conf[i]!=0)  
  pulse = conf[i++] * 10 ;
else 
{
  orscV2.resetDecoder();		
  i=0;
  j++;
       if (j==1)
    conf = ConfZ1;
  else if (j==2)
    conf = ConfZ2;
  else if (j==3)
    conf = ConfZ3;
  else if (j==4)
    conf=Z2Eco ;    
  else if (j==5)
    conf=Z2Conf;    
  else if (j==6)
    conf=Z3Eco ;    
  else if (j==7)
    conf=Z3Conf;    
    
}
}

char * state [] = {
"UN",
"T0", 
"T1", 
"T2", 
"T3", 
"OK", 
"DO" };

void printState()
{
  Serial.print("NbPulse ");
  Serial.print(NbPulse,10);

  Serial.print(" state ");
  Serial.print(state[orscV2.state]); 

  Serial.print(" Bits ");
  Serial.print(orscV2.total_bits,10);

  Serial.print(" p:");
  Serial.print(ConfZ1[i-1],10);

  Serial.println();
  

}
void loop () {
    setPulse();
    cli();
    word p = pulse;
    
    pulse = 0;
    sei();
      
    if (p != 0) {
        NbPulse++;
        if (orscV2.nextPulse(p))
        {
            reportSerial("OSV2", orscV2);  
            orscV2.reportSerial( "heger" ) ;
            LastReceive = millis() ;
    	    NbReceive ++ ;     
            NbPulse=0 ;
          //data valide checksum
          if (orscV2.IsValid () )
          {
           Serial.println("checksum Ok");
          }
          else
          {
           Serial.println("Bad checksum");
          }
           orscV2.resetDecoder();		
                
        }
        printState();    
    }

}


