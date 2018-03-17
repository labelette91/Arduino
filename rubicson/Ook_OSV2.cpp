#if _MSC_VER 

#include "VSPDE.h"
#include <string>
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
int conf [1000] ;

//{ 525, 1725, 425, 3600, 425, 1725, 425, 3600, 425, 3625, 425, 1725, 425, 3600, 425, 1725, 425, 1725, 425, 1700, 425, 3600, 425, 3600, 425, 3600, 425, 1725, 425, 1725, 425, 1725, 425, 1725, 425, 1725, 400, 1725, 425, 3600, 425, 1725, 425, 1725, 425, 1725, 425, 3600, 400, 1725, 425, 1725, 425, 3625, 400, 1725, 425, 1725, 425, 1750, 400, 3600, 425, 1725, 400, 1750, 400, 3625, 425, 1725, 400, 1725, 425 , 0 };

//{ 540 ,1890 ,540 ,3780 ,540 ,1890 ,540 ,3780 ,540 ,3780 ,540 ,3780 ,540 ,3780 ,540 ,3780 ,540 ,1890 ,540 ,1890 ,540 ,3780 ,540 ,3780 ,540 ,3780 ,540 ,1890 ,540 ,1890 ,540 ,1890 ,540 ,1890 ,540 ,1890 ,540 ,1890 ,540 ,1890 ,540 ,3780 ,540 ,3780 ,540 ,1890 ,540 ,1890 ,540 ,4050 ,540 ,1890 ,540 ,4050 ,540 ,4050 ,540 ,1890 ,540 ,1890 ,540 ,4050 ,540 ,1890 ,540 ,3780 ,540 ,1890 ,540 ,3780 ,540 ,3780 ,540 ,9180 ,0 };
/*
There first 8 pulses are the header and the last 2 pulses are the footer. These are meant to identify the pulses as genuine. We don’t it for further processing. The next step is to transform this output into 36 groups of 2 pulses (and thereby dropping the footer pulses).

540 1890   5
540 3780
540 1890
540 3780

540 3780   F
540 3780
540 3780
540 3780

540 1890   3
540 1890
540 3780
540 3780

540 3780 8
540 1890
540 1890
540 1890

540 1890 0
540 1890
540 1890
540 1890

540 3780 C
540 3780
540 1890
540 1890

540 4050 B
540 1890
540 4050
540 4050

540 1890 2
540 1890
540 4050
540 1890

540 3780 B
540 1890
540 3780
540 3780

If we now look at carefully at these groups you can distinguish two types of groups:

    540 1890
    540 3780

So the first group is defined by a low 2nd, the second group has a high 2nd pulse. So we take either of these two pulses to define a 0 or a 1. In this case we say a high 2nd pulse means a 1 and a low 2nd pulse means a 0. We then get the following output:

0101 1111 0011 0000 0001 1001 0110 0101 0111

Each (group) of numbers has a specific meaning:

    Header 0 till 3
    ID: 4 till 11
    Battery: 12
    TX mode: 13
    Channel: 14 till 15
    Temperature: 16 till 27
    Humidity: 28 till 35

0101 11110011 0 0 00 000110010110 01010111
5    F    3    
0101 1111 0011 0000 0001 1001 0110 0101 0111

    The ID is defined as a binary number

    The Battery identifies the state of the battery
    The TX mode defines whether the signal was sent automatic or manual
    The Channel is defined as a binary number and specifies which channel the sensor uses
    The Temperature is defined as a binary number and represents the temperature
    The Humidity is defined as a binary number and represents the humidity

This protocol was created for pilight with the help of this thread: http://forum.pilight.org/Thread-Fully-Supported-No-brand-temp-humidity-sensor
*/

char * config = 

"540 1890 540 3780 540 1890 540 3780 540 3780 540 3780 540 3780 540 3780 540 1890 540 1890 540 3780 540 4050 540 3780 540 1890 540 1890 540 1890 540 1890 540 1890 540 1890 540 1890 540 3780 540 3780 540 1890 540 1890 540 4050 540 3780 540 1890 540 4050 540 1890 540 1890 540 4050 540 1890 540 3780 540 1890 540 3780 540 3780 540 9180";

/*
- first 4 bits (1-4) are always 0101

- next 8 bits (5-12) are device id

- next 1 bit (13) I assume is battery, 1 = good, 0 = low

- next 1 bit (14) is tx mode, 0 = auto 1 = manual

- next 2 bits (15-16) are channel setting (00 = channel 1, 01 = channel 2, 10 = channel 3)

- next 12 bits (17-28) are temp (e.g. 540 1890 540 1890 540 1890 540 1890 540 3780 540 3780 540 1890 540 1890 540 4050 540 1890 540 4050 540 4050 = 000011001011 = 20.3)

- next 8 bits (29-36) are humidity (e.g. 540 1890 540 1890 540 4050 540 1890 540 3780 540 1890 540 3780 540 3780 = 00101011 = 43%)

- Last bit (37), or two pulses (e.g. 540 9180) seems to be the footer
*/
void setPulse()
{
  int nb=0;

  std::string list[1000];
if (conf[0]==0)
{
  Split ( config  , " " , "" , true , list ) ;
  while (list[nb]!="")
  {
    conf[nb] = atoi (list[nb].c_str());
    nb++;
  }
  
}

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
        printState();    
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


