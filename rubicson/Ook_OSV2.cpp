#if _MSC_VER 

#include "VSPDE.h"
#endif



class DecodeOOK {
public:
    byte total_bits, bits, state, pos, data[25];

    virtual char decode (word width, byte level ) {return -1;};
    
public:

    enum { UNKNOWN, T0, T1, T2, T3, OK, DONE };

    DecodeOOK () { resetDecoder(); }

    bool nextPulse (word width, byte level) {
        if (state != DONE)
        
            switch (decode(width,level)) {
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
        total_bits = bits = pos =  0;
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
    
    
    void done () {
        while (bits)
            gotBit(0); // padding
        state = DONE;
    }
};

#include "rubicson.h"

DecodeRubicson orscV2;
 
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

#ifdef  INT_EXTERNE
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

	for (byte i = 0; i < pos; ++i) {
		byte bt = data[i];
		for (byte b = 0; b < 8; b++) {
			if (bt & 0x80)
				Serial.print('1');
			else
				Serial.print('0');
			bt = bt << 1;
			if (b == 3) Serial.print(' ');

		}

		Serial.print(' ');
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


int i=0;
int j=1;
int conf []=

{ 525, 1725, 425, 3600, 425, 1725, 425, 3600, 425, 3625, 425, 1725, 425, 3600, 425, 1725, 425, 1725, 425, 1700, 425, 3600, 425, 3600, 425, 3600, 425, 1725, 425, 1725, 425, 1725, 425, 1725, 425, 1725, 400, 1725, 425, 3600, 425, 1725, 425, 1725, 425, 1725, 425, 3600, 400, 1725, 425, 1725, 425, 3625, 400, 1725, 425, 1725, 425, 1750, 400, 3600, 425, 1725, 400, 1750, 400, 3625, 425, 1725, 400, 1725, 425 , 0 };

void setPulse()
{

if ( conf[i]!=0)  
  pulse = conf[i++]  ;
else 
{
  orscV2.resetDecoder();		
    
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

  Serial.print(": ");
  for (byte i = 0; i < 5;i++)
	  Serial.print(orscV2.data[i] , HEX);


  Serial.println();
  

}
void loop () {
  byte level;
    setPulse();
    cli();
    word p = pulse;
    
    pulse = 0;
    sei();
      
    if (p != 0) {

		NbPulse++;
        level = (NbPulse&1)==1 ;

		Serial.print("(");
		Serial.print(p, 10);
		Serial.print(",");
		Serial.print(level,10);
		Serial.print(")");

        if (orscV2.nextPulse(p,level))
        {
            reportSerial("OSV2", orscV2);  
			orscV2.ReportSerial();
            LastReceive = millis() ;
    	    NbReceive ++ ;     
            NbPulse=0 ;
           orscV2.resetDecoder();		
                
        }
        printState();    
    }

}


