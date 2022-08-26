

// 433 MHz decoders

//#include "DecodeOOK.h"
uint8_t lfsr_digest8(uint8_t const message[], unsigned bytes, uint8_t gen, uint8_t key);
uint16_t getRaw16bValue(uint8_t* data, uint8_t offset, uint8_t size);
uint8_t getRaw08bValue(uint8_t* data, uint8_t offset, uint8_t size);

class Hideki : public DecodeOOK {
public:
    unsigned long LastReceivedTime ;

    Hideki() : DecodeOOK () {
        resetDecoder(); 
        PacketCountSeuil=1;
        Name="T";
    
    }
    
    // add one bit to the packet data buffer
    virtual void gotBit (char value) {
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
          if ( (data[0] == 0x01 ) &&  (data[1] == 0x45 ) )
            max_bits = (7*8); // TFA3208
          else
          {
            resetDecoder();
            return;
          }
        }
        if (pos >= sizeof data) {
            resetDecoder();
            return;
        }
        state = OK;
    }
    virtual char decode (word width, byte BitData) {
        if (400 <= width && width < 1100 ) 
        {
            byte w = width >= 750;
            switch (state) {
                case UNKNOWN:
                    if (w == 0) {
                        // short pulse
                        ++flip;
                    } else if (w == 1 && flip >= 11 ) {
//                    } else if (w == 1 && flip >= 15 ) {
                        // long pulse, start bit
                        flip = 0;
                        data[0]=0;
                         manchester(0);
                         total_bits=7;
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
        } else if ((width >= 2500)  && (total_bits == max_bits) ) {
            return 1;
        } else {
            return -1;
        }
//        printf( "%d %d %d %d \n",state , total_bits , bits , flip); 
        return total_bits == max_bits ? 1: 0;
    }
byte gethumidity (byte* data)
{
    return data[5] ;
}	
byte gethumidity ()
{
    return gethumidity(data);
}
byte getChannel (byte* data)
{

    byte iCanal ; //le canal du satellite

    iCanal      = ((data[3] & 0x70) >> 4) + 1;

    return (iCanal ) ;
}	
byte getChannel ()
{
    return getChannel(data);
}
float getTemperature (byte* data)
{
	float fTemp; // Température
    int Temp   = ((data[3] & 0x0f) << 8) | data[4]; 
//    int Temp = getRaw16bValue(data,28,12);
	//convert T = (X - 720) * 0.0556
	fTemp = (Temp-720)* 0.0556;
    return fTemp;	
}	
float getTemperature ()
{
    return getTemperature(data);
}
//bit 18 
// l'etat de la baterie : 0 = faible 15= 100%
byte getBatteryLevel()
{
    //(b[2] & 0x80) != 0; // if not zero, battery is low
    return ((data[3] & 0x80) ) ? 0 : 15; ;
}
byte getId()
{
    byte id = data[2] ;
    return id;
}
byte getCrc()
{
    byte crc = data[6] ;
    return crc;
}
unsigned long DeltaReceivedTime()
{
    register unsigned long ms = millis() ;
    register unsigned long delta  = ms - LastReceivedTime;
    LastReceivedTime = ms ;
    return delta;
}
void ReportSerial()
 {
					byte Id    = getId();
					byte Canal = getChannel();
					float Temp = getTemperature();
					byte Hum   = gethumidity();
                    byte bat   = getBatteryLevel();
                    printHexa ( data, total_bits/8);
 //                   printBinary ( data, total_bits/8 , 8 );
					Serial.print(" TFA:");
					Serial.print(Id);
					Serial.print(" " );
					Serial.print(Canal);
					Serial.print(" " );
					Serial.print(Temp);
					Serial.print(" " );
					Serial.print(Hum);
					Serial.print(" " );
					Serial.println(bat);
 }
  
  
  virtual bool isValid()
  {
      uint8_t expected = data[6];
      uint8_t calculated = lfsr_digest8((uint8_t const*)&data[1], 5, 0x98, 0x3e) ^ 0x64;
      if (expected != calculated) {
          return false;
      }
      return true;
  }

};
void PulseLed();
 void managedHideki(Hideki* ptfa3208 , word p)
 {
    if (p != 0) 
    {
      if (ptfa3208->nextPulse(p))
      {
        if (ptfa3208->newPacket())
        {// ce sont bien nos sondes (signature, identification dans le 1er octet du header
          {
            PulseLed();
            
 #ifndef DOMOTIC
//              if (ptfa3208->newPacket())  Serial.print("NEW ");
                Serial.print(ptfa3208->DeltaReceivedTime());                Serial.print(" ");
                ptfa3208->ReportSerial();
 #else
              reportDomoticTempHum (ptfa3208->getTemperature()*10 , ptfa3208->gethumidity(), ptfa3208->getId(), ptfa3208->getChannel(), ptfa3208->getBatteryLevel(),sTypeTH11_TFA);
 #endif
          }
        }
        ptfa3208->resetDecoder(); 
      }
    }
 }
