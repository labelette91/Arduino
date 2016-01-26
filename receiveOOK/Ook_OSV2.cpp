// Oregon V2 decoder added - Dominique Pierre
// Oregon V3 decoder revisited - Dominique Pierre
// New code to decode OOK signals from weather sensors, etc.
// 2010-04-11 <jcw@equi4.com> http://opensource.org/licenses/mit-license.php
// $Id: ookDecoder.pde 5331 2010-04-17 10:45:17Z jcw $

class DecodeOOK {
protected:
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

// 433 MHz decoders


class OregonDecoderV2 : public DecodeOOK {
public:
    OregonDecoderV2() {}
    
    // add one bit to the packet data buffer
    virtual void gotBit (char value) {
        if(!(total_bits & 0x01))
        {
            data[pos] = (data[pos] >> 1) | (value ? 0x80 : 00);
        }
        total_bits++;
        pos = total_bits >> 4;
        if (pos >= sizeof data) {
            resetDecoder();
            return;
        }
        state = OK;
    }
    
    virtual char decode (word width) {
        if (200 <= width && width < 1200) {
            byte w = width >= 700;
            switch (state) {
                case UNKNOWN:
                    if (w != 0) {
                        // Long pulse
                        ++flip;
//                    } else if (32 <= flip) {
                    } else if (w == 0 && 24 <= flip) {
                        // Short pulse, start bit
                        flip = 0;
                        state = T0;
                    } else {
                      // Reset decoder
                        return -1;
                    }
                    break;
                case OK:
                    if (w == 0) {
                        // Short pulse
                        state = T0;
                    } else {
                        // Long pulse
                        manchester(1);
                    }
                    break;
                case T0:
                    if (w == 0) {
                      // Second short pulse
                        manchester(0);
                    } else {
                        // Reset decoder
                        return -1;
                    }
                    break;
            }
        } else if (width >= 2500  && pos >= 8) {
            return 1;
        } else {
            return -1;
        }
        return total_bits == 160 ? 1: 0;
    }
};


OregonDecoderV2 orscV2;


byte checksum(const byte* data)
{
  int c = ((data[6]&0xF0) >> 4) + ((data[7]&0xF)<<4);
  int s = ((Sum(6, data) + (data[6]&0xF) - 0xa) & 0xff);
  return s == c;
}
 
byte checksum2(const byte* data)
{
  return data[8] == ((Sum(8, data) - 0xa) & 0xff);
}
 
int Sum(byte count, const byte* data)
{
  int s = 0;
 
  for(byte i = 0; i<count;i++)
  {
    s += (data[i]&0xF0) >> 4;
    s += (data[i]&0xF);
  }
 
  if(int(count) != count)
    s += (data[count]&0xF0) >> 4;
 
  return s;
}
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
float temperature(const byte* data)
{
    int sign = (data[6]&0x8) ? -1 : 1;
    float temp = ((data[5]&0xF0) >> 4)*10 + (data[5]&0xF) + (float)(((data[4]&0xF0) >> 4) / 10.0);
    return sign * temp;
}
//temperature sur un int multiplier par 10
int temperatureint(const byte* data)
{
    int sign = (data[6]&0x8) ? -1 : 1;
    int temp = ((data[5]&0xF0) >> 4)*10 + (data[5]&0xF) ;
    temp = temp*10 + ((data[4]&0xF0) >> 4) ;
    return sign * temp;
}
int temperatureEnt(const byte* data)
{
    int sign = (data[6]&0x8) ? -1 : 1;
    int temp = ((data[5]&0xF0) >> 4)*10 + (data[5]&0xF) ;
    return sign * temp;
}
int temperatureFrac(const byte* data)
{
    int temp = ((data[4]&0xF0) >> 4) ;
    return temp;
}




byte getHumidity(const byte* data)
{
    return (data[7]&0xF) * 10 + ((data[6]&0xF0) >> 4);
}
 
// Ne retourne qu'un apercu de l'etat de la baterie : 10 = faible
byte battery(const byte* data)
{
    return (data[4] & 0x4) ? 10 : 90;
}
 
byte channel(const byte* data)
{
    byte channel;
    switch (data[2])
    {
        case 0x10:
            channel = 1;
            break;
        case 0x20:
            channel = 2;
            break;
        case 0x40:
            channel = 3;
            break;
     }
 
     return channel;
}

byte getId (const byte* data)
{
  return data[3];
}

//domotic structure
	struct T_TEMP {
		byte	packetlength;
		byte	packettype;
		byte	subtype;
		byte	seqnbr;
		byte	id1;
		byte	id2;
		byte	temperatureh : 7;
		byte	tempsign : 1;
		byte	temperaturel;
		byte	battery_level : 4;
		byte	rssi : 4;
	} ;
	
#define BYTE byte
	
	struct T_TEMP_HUM{
		BYTE	packetlength;
		BYTE	packettype;
		BYTE	subtype;
		BYTE	seqnbr;
		BYTE	id1;
		BYTE	id2;
		BYTE	temperatureh : 7;
		BYTE	tempsign : 1;
		BYTE	temperaturel;
		BYTE	humidity; 
		BYTE	humidity_status;
		BYTE	battery_level : 4;
		BYTE	rssi : 4;
	} ;
	

struct T_TEMP Temp  ;
struct T_TEMP_HUM Temp_Hum  ;

void reportDomotic ( class DecodeOOK& decoder){
    byte pos;
    int temp ;
    const byte* data = decoder.getData(pos);
		Temp.packetlength = 8;
		Temp.packettype   = 0x50;
		Temp.subtype      = 05  ;
		Temp.seqnbr       = 0x2e;
		Temp.id1          = 0x48;
		Temp.id2          = 0x00;
		
		temp = temperatureint(data);
		Temp.temperatureh = 0   ;
		if (temp>=0)
			Temp.tempsign     = 0   ;
		else
			Temp.tempsign     = 1   ;
		Temp.temperaturel   = temp & 0xff;
		Temp.battery_level  = 6;
		Temp.rssi           = 9 ;
            
    Serial.write((byte*)&Temp,9);
    decoder.resetDecoder();		
}

void reportDomoticTempHum ( class DecodeOOK& decoder){
  byte pos;
  int temp ;
  const byte* data = decoder.getData(pos);
  Temp_Hum.packetlength = sizeof(Temp_Hum)-1;
  Temp_Hum.packettype   = 0x52;   //pTypeTEMP_HUM
  Temp_Hum.subtype      = 01  ;   //sTypeTH1 0x1  //THGN122/123,THGN132,THGR122/228/238/268
  Temp_Hum.seqnbr       = 0x2e;
  Temp_Hum.id1          = 0x47;
  Temp_Hum.id2          = 0x00;
  
  temp = temperatureint(data);
  Temp_Hum.temperatureh = 0   ;
  if (temp>=0)
  	Temp_Hum.tempsign     = 0   ;
  else
  	Temp_Hum.tempsign     = 1   ;
   Temp_Hum.temperaturel   = temp & 0xff;
  Temp_Hum.battery_level  = 6;
  Temp_Hum.rssi           = 9 ;
  Temp_Hum.humidity       = getHumidity(data) ;
  Temp_Hum.humidity_status= 0  ;
  
  
  Serial.write((byte*)&Temp_Hum,sizeof(Temp_Hum));
              
  decoder.resetDecoder();		
}


void reportSerialAscii (const char* s, class DecodeOOK& decoder) {
    byte pos;
    const byte* data = decoder.getData(pos);
    Serial.print(s);
    Serial.print(' ');
    Serial.print(millis() / 1000);
    Serial.print(" Dt:");
    Serial.print(Dt);
    Serial.print(" Nb:");
    Serial.print(NbReceive);
    Serial.print(" Np:");
    Serial.print(NbPulse);
    Serial.print(' ');



    for (byte i = 0; i < pos; ++i) {
        Serial.print(data[i] >> 4, HEX);
        Serial.print(data[i] & 0x0F, HEX);
    }
    
//    Serial.println();
 // Outside/Water Temp : THN132N,...
    if(data[0] == 0xEA && data[1] == 0x4C)
    {
       Serial.print(" [THN132N,...] Id:");
       Serial.print(data[3], HEX);
       Serial.print(" ,Channel:");
       Serial.print(channel(data));
       Serial.print(" ,temp:");
       Serial.print(temperature(data));
       Serial.print(" ,temp:");
       Serial.print(temperatureint(data));
       Serial.print(" ,bat:");
       Serial.print(battery(data));
       Serial.println();
    }
    // Inside Temp-Hygro : THGR228N,...
    else if(data[0] == 0x1A && data[1] == 0x2D)
    {
       Serial.print(" [THGR228N,...] Id:");
       Serial.print(data[3], HEX);
       Serial.print(" ,Channel:");
       Serial.print(channel(data));
       Serial.print(" ,temp:");
       Serial.print(temperature(data));
       Serial.print(" ,temp:");
       Serial.print(temperatureint(data));
       Serial.print(" ,temp:");
       Serial.print(temperatureEnt(data));
       Serial.print(".");
       Serial.print(temperatureFrac(data));


       Serial.print(" ,hum:");
       Serial.print(getHumidity(data));
       Serial.print(" ,bat:");
       Serial.print(battery(data));
       Serial.println();
    }    
    decoder.resetDecoder();
}

void reportSerial (const char* s, class DecodeOOK& decoder) 
{
if (Dt!=0)
{

#ifndef DOMOTIC
            reportSerialAscii("OSV2", decoder);  
#else            
//            reportDomotic( decoder);  
            reportDomoticTempHum ( decoder);
#endif      
} 
}

#define PRINT_REG(reg,base) Serial.print (#reg);Serial.print (':'); Serial.println(reg,base);

void PrintRegister()
{
#ifndef DOMOTIC
    PRINT_REG(ACSR,BIN);
    PRINT_REG(ADCSRA,BIN);
    PRINT_REG(ADCSRB,BIN);
    PRINT_REG(DIDR1,BIN);
    PRINT_REG(DDRD,BIN);
    PRINT_REG(PORTD,BIN);
    PRINT_REG(WDTCSR,BIN);
#endif
  
}

void setup () {
	  LastReceive = 0 ;
	  NbReceive   = 0;
    
    Serial.begin(38400);
//    Serial.println("\n[ookDecoder]");
    
   // initialize the LED pin as an output:
    pinMode(ledPin, OUTPUT);       
    
    pulse=0;
#ifndef INT_EXTERNE
/*
    pinMode(13 + PORT, INPUT);  // use the AIO pin
    digitalWrite(13 + PORT, 1); // enable pull-up

    // use analog comparator to switch at 1.1V bandgap transition
    ACSR = _BV(ACBG) | _BV(ACI) | _BV(ACIE);

    // set ADC mux to the proper port
    ADCSRA &= ~ bit(ADEN);
    ADCSRB |= bit(ACME);
    ADMUX = PORT - 1;
*/

//input sur AIN1
// AIN0 = bandgap ref

//ACME = 0 Analog Comparateur Multiplexer enable
    bitClear(ADCSRB,ACME);
//aden = 1 
    bitSet(ADCSRA,ADEN);
    
    // use analog comparator to switch at 1.1V bandgap transition
    bitSet(ACSR , ACBG );
    bitSet(ACSR , ACI  );
    bitSet(ACSR , ACIE );

//interrupt mode toggle
    bitClear(ACSR , ACIS1 );
    bitClear(ACSR , ACIS0 );


    pinMode(AIN1, INPUT_PULLUP);  // use the AIO pin
//    digitalWrite(AIN1 , 1); // enable pull-up


#else
   attachInterrupt(1, ext_int_1, CHANGE);
   // initialize the pushbutton pin as an input:
 
 //enable pull up 
 //PORTD : pullup  1 = active 0: tri state
 PORTD |= 0x08;
 //DDRD : direction register : 1 = output 0 : input
 

//   DDRE  &= ~_BV(PE5);
//   PORTE &= ~_BV(PE5);
#endif

PrintRegister();
}

void PulseLed()
{
    if (Dt!=0)
    {
      if (digitalRead(ledPin)== LOW) 
         digitalWrite(ledPin, HIGH);  
      else
         digitalWrite(ledPin, LOW);  
    }

}
void loop () {
    cli();
    word p = pulse;
    
    pulse = 0;
    sei();
      
    if (p != 0) {
        NbPulse++;
        if (orscV2.nextPulse(p))
        {
          //data valide checksum
          if (checksum2(orscV2.getData()) )
          {
            Dt = (millis() - LastReceive)/1000;
            PulseLed();
            reportSerial("OSV2", orscV2);  
            LastReceive = millis() ;
    	    NbReceive ++ ;     
            NbPulse=0 ;
 
          }
          else
          {
#ifndef DOMOTIC
           Serial.println("Bad checksum");
#endif     
          }
           orscV2.resetDecoder();		
          
        }
/*        if (orscV3.nextPulse(p))
            reportSerial("OSV3", orscV3);        */

    }

}

