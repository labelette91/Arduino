
class DecodeOOK {
public:
    byte total_bits, max_bits,bits, flip, state, pos, data[25];
    char sbits[100];
//    String Spaquet; //tous les bits dans une String
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
        max_bits = 160;
//        Spaquet="";
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


class Hideki : public DecodeOOK {
public:
    Hideki() {}
    
    // add one bit to the packet data buffer
    virtual void gotBit (char value) {
        sbits[total_bits]=value + '0';
        sbits[total_bits+1]=0 ;
//        if (value) Spaquet += '1';  else  Spaquet += '0';


//        data[pos] = (data[pos] >> 1) | (value ? 0x80 : 00);
        data[pos] = (data[pos] << 1) | (value );

        total_bits++;
        pos = total_bits >> 3 ;
        /*compute size of paxket */
        if(pos == 2)
        {
          // Taille de trame par défaut (utilisée dans la majorité des sondes)
          max_bits = 160;
 

          // tfa3208
          if(data[0] == 81 )
            max_bits = (6*8); // TFA3208
/*
          else if(data[0] == 0xEA)
          {
            if(data[1] == 0x4C) max_bits = 136; // TH132N : 68 * 2
            else if(data[1] == 0x7C) max_bits = 240; // UV138 : 120 * 2
          }
          else if(data[0] == 0x5A)
          {
            if(data[1] == 0x5D) max_bits = 176; // THGR918 : 88 * 2
            else if(data[1] == 0x6D)max_bits = 192; // BTHR918N : 96 * 2
          }
          else if(data[0] == 0x1A  && data[1] == 0x99)
            max_bits = 176; // WTGR800 : 88 * 2
          else if(data[0] == 0xDA  && data[1] == 0x78)
            max_bits = 144; // UVN800 : 72 * 2
          else if((data[0] == 0x8A || data[0] == 0x9A) && data[1] == 0xEC)
            max_bits = 208; // RTGR328N 104 * 2
          else if(data[0] == 0x2A)
          {  
            if(data[1] == 0x19) max_bits = 184; // RCR800 : 92 * 2
            else if(data[1] == 0x1d) max_bits = 168; // WGR918 : 84 * 2
          }
*/          
        }
         
        /* */
        if (pos >= sizeof data) {
            resetDecoder();
            return;
        }
        state = OK;
    }
    
    virtual char decode (word width) {
//        if (200 <= width && width < 1200) {
        if (400 <= width && width < 1100 ) 
        {
            byte w = width >= 750;
            switch (state) {
                case UNKNOWN:
                    if (w == 0) {
                        // short pulse
                        ++flip;
                    } else if (w == 1 && flip >= 15 ) {
                        // long pulse, start bit
                        flip = 0;
                         manchester(0);
                        state = OK;
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
        return total_bits == max_bits ? 1: 0;
    }
byte GetHum (byte* data)
{
	byte  iHum; // humidité
	
	//hum 8 bits bits 34 .. 41 6 bit data[4] + 2 bits data[5]
	iHum = data[4] & 0x3F ;
	iHum <<= 2 ;
	iHum += data[5] >> 6 ;
    return iHum;
}	

byte GetHum ()
{
    return GetHum(data);
}

byte GetCanal (byte* data)
{
/*	iCanal = Bin2Dec(Spaquet.substring(19, 22));
    fTemp = (Bin2Dec(Spaquet.substring(22, 34)) - 720) * 0.0556;
    fTemp *= 10;
    fTemp = int(fTemp + 0.5);
    fTemp /= 10;
    iHum = Bin2Dec(Spaquet.substring(34, 42));
*/	
	byte iCanal ; //le canal du satellite

	//bit 19 a 21
	iCanal  = data[2] >> 2;

    return iCanal & 0x3 ;
}	
byte GetCanal ()
{
    return GetCanal(data);
}


float GetTemp (byte* data)
{
	float fTemp; // Température

	//temp bit 22 a 33 : 12 bit en faraneight :  2 bit  data[2] + 8 bits  data[3] + 2 bits  data[4]
	word 
	Temp   = data[2] & 0x3 ;
	Temp <<=  8 ;
	Temp += data[3] ;
	Temp <<=  2 ;
	Temp += ( data[4] >>6 ) ;

	//convert T = (X - 720) * 0.0556
	fTemp = (Temp-720)* 0.0556;
    return fTemp;	
}	
float GetTemp ()
{
    return GetTemp(data);
}
	

};

