

// 433 MHz decoders

//#include "DecodeOOK.h"

class Hideki : public DecodeOOK {
public:
    unsigned long LastSend ;
    //last packet data received ident
    byte lastdata[6] ;
    byte numberSamePacketReceided = 0 ;
    unsigned long LastReceivedTime ;

    Hideki() : DecodeOOK () {}
    
    // add one bit to the packet data buffer
    virtual void gotBit (char value) {
//        sbits[total_bits]=value + '0';
//        sbits[total_bits+1]=0 ;
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
          else
          {
            resetDecoder();
            return;
          }

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
                    } else if (w == 1 && flip >= 11 ) {
//                    } else if (w == 1 && flip >= 15 ) {
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
        } else if ((width >= 2500)  && (total_bits == max_bits) ) {
            return 1;
        } else {
            return -1;
        }
        return total_bits == max_bits ? 1: 0;
    }
byte gethumidity (byte* data)
{
	byte  iHum; // humidité
	
	//hum 8 bits bits 34 .. 41 6 bit data[4] + 2 bits data[5]
	iHum = data[4] & 0x3F ;
	iHum <<= 2 ;
	iHum += data[5] >> 6 ;
    return iHum;
}	

byte gethumidity ()
{
    return gethumidity(data);
}

byte getChannel (byte* data)
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

    return (iCanal & 0x3) + 1 ;
}	
byte getChannel ()
{
    return getChannel(data);
}


float getTemperature (byte* data)
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
float getTemperature ()
{
    return getTemperature(data);
}
//bit 18 
// l'etat de la baterie : 0 = faible 15= 100%
byte getBatteryLevel()
{
    return ((data[2]>>5)  & 1 ) ? 0 : 15; ;
}

word getId()
{

    byte id = data[1] & 0x3F;
    id <<=2 ;
    id += (data[2]>>6) ;
    return id;
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
#ifndef WIN32			
                    printHexa ( data, total_bits/8);
//                    printBinary ( data, total_bits/8 , 8 );
					resetDecoder(); 

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

#else
for (byte i=0;i<total_bits/8;i++) printf("%02X",data[i]); 
printf("Décodage : ");
printf("%d ",getId());
printf("%d",getChannel());
printf(" " );
printf("%f",getTemperature());
printf(" " );
printf("%d\n",gethumidity());
					resetDecoder(); 

#endif
 }

//return true si new packet    
  bool newPacket()
  {
    //send at least every 10 min
 //   register unsigned long time = millis() ;
	//if (  (time - LastSend) > (10*60*1000)  ) 
 //   {
 //     lastdata[5] = 0;
 //   }

      //ignore 2 low temp bits 
    if ((lastdata[5] != data[5]) || (lastdata[3] != data[3]) || ( ( lastdata[4] & 0x3F)  != (data[4] & 0x3F) ) ||  (lastdata[1] != data[1]) || (lastdata[2] != data[2])) {
      lastdata[1] = data[1];
      lastdata[2] = data[2];
      lastdata[3] = data[3];
      lastdata[4] = data[4];
      lastdata[5] = data[5];
//      LastSend =time ;

      return true;
    }

    return false ;

  }
    bool valid()
    {
        byte hum = gethumidity();
        if ( (hum < 20 ) || (hum > 99 ) ) 
            return false;
        float temp = getTemperature();
        if ( (temp < -20.0 ) || (temp > 50.0 ) ) 
            return false;
        return true ;
    }

  virtual bool nextPulse1 (word width) {
    bool receivedOk = DecodeOOK::nextPulse(width);
    if (receivedOk)
	{
      if (!valid()) 
	  {
          receivedOk = false ;
          resetDecoder(); 
 #ifndef DOMOTIC
          Serial.println("TFA BAD");
 #endif 
      }
    }
    return receivedOk;
  }


    virtual bool nextPulse (word width) 
    {
        bool receivedOk = DecodeOOK::nextPulse(width);
        //if pcket received
        if (receivedOk)
        {
            //if packet valid
//            if (valid() )
            if (1 )
            {
                numberSamePacketReceided ++ ;
        
                Serial.print("sp"); Serial.print( numberSamePacketReceided );
                if ( (numberSamePacketReceided>1) &&  (newPacket() ) )
                {
                    numberSamePacketReceided = 1 ;
                }
                //2 packet identique
                Serial.print(" "); Serial.print( numberSamePacketReceided );
                if (numberSamePacketReceided>=2)
                {
                    receivedOk = true;
                    numberSamePacketReceided=0;
                }
                else
                {
                    receivedOk = false;
                }
                Serial.print(" "); Serial.print( numberSamePacketReceided );
                ReportSerial();
            }
            else
                receivedOk = false;
            
            if (receivedOk == false )
            {
                resetDecoder(); 
            }
        }
        return receivedOk ;
    }

};
void PulseLed();
 void managedHideki(Hideki* ptfa3208 , word p)
 {
    if (p != 0) 
    {
      if (ptfa3208->nextPulse(p))
      {
        {// ce sont bien nos sondes (signature, identification dans le 1er octet du header
          {
            PulseLed();
            
 #ifndef DOMOTIC
//              if (ptfa3208->newPacket())  Serial.print("NEW ");
//                Serial.print(ptfa3208->DeltaReceivedTime());                Serial.print(" ");
                ptfa3208->ReportSerial();
 #else
              reportDomoticTempHum (ptfa3208->getTemperature()*10 , ptfa3208->gethumidity(), ptfa3208->getId(), ptfa3208->getChannel(), ptfa3208->getBatteryLevel());
 #endif
          }
        }
        ptfa3208->resetDecoder(); 
      }
    }
 }
