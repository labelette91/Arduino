//minimum pulse in micros
#define PULSE_MIN 100
//maximum pulse in micros
#define PULSE_MAX 700
//short pulse duration 
#define PULSE_SHORT 300
//preamble size in pulse
#define PREAMBLE  32
// numeber of message bits
#define MESSAGE_BIT  65

class HagerDecoder : public DecodeOOK {
public:
    HagerDecoder() {}
    
    // add one bit to the packet data buffer
    virtual void gotBit (char value) {
        //ignore first bit
        if (total_bits>0)
          data[pos] = (data[pos] << 1) | (value ? 0x1 : 00);
        total_bits++;
        pos = (total_bits-1)>>3;

        if (pos >= sizeof data) {
            resetDecoder();
            return;
        }
        state = OK;
    }
    
    virtual char decode (word width) {
        if (PULSE_MIN <= width && width < PULSE_MAX) {
            byte w = width >= PULSE_SHORT;
            switch (state) {
                case UNKNOWN: //wait preamble
                    if (w != 0) { // Long pulse : , start bit : wait 
                        if ( flip >= PREAMBLE )
                          state = T1;
                        flip = 0;
                    } else if (w == 0 ) { // Short pulse
                        ++flip;
                    } 
                    break;
                case T1: //wait 2 long
                if (w != 0) {
                      flip++;
                      if (flip>=1){
                        state = T2;
                        flip = 0;
                      }
                } else   // Reset decoder
                    return -1;
                break;
                case T2: //wait //2 short
                if (w == 0) {
                      flip++;
                      if (flip>=2){
                        state = T3;
                        flip = 0;
                      }
                } else   // Reset decoder
                    return -1;
                break;
                case T3: //wait //2 long
                if (w != 0) {
                      flip++;
                      if (flip>=2){
                        state = T0;
                        flip = 0;
                      }
                } else   // Reset decoder
                    return -1;
                break;
                
                case OK:
                    if (w == 0) {
                        // Short pulse
                        state = T0;
                    } else {
                        // Long pulse
                        manchester(1);
                        state = OK;
                    }
                    break;
                case T0:
                    if (w == 0) { // Second short pulse
                        manchester(0);
                    } else   // Reset decoder
                      return -1;
                    break;
            }
        } else {
            return -1;
        }
        return total_bits == MESSAGE_BIT ? 1: 0;
    }

    byte Sum()
    {
      byte s = 0;
     
      for(byte i = 0; i<total_bits>>3;i++)
      {
        s += data[i];
      }
      return (s);
    }

    bool IsValid() 
    {
      return ( Sum()==0);
    }

    void reportSerial( char* s ) {
    Serial.print(s);
    Serial.print(' ');

    for (byte i = 0; i < pos; ++i) {
        Serial.print(data[i] >> 4, HEX);
        Serial.print(data[i] & 0x0F, HEX);
        if (i==3) Serial.print(' ');
    }
    
    Serial.println();
}
    
};
