//#######################################################################################################
//##                    This Plugin is only for use with the RFLink software package                   ##
//##                                        Plugin-35 Imagintronix                                     ##
//#######################################################################################################
/*********************************************************************************************\
 * This plugin takes care of decoding Imagintronix sensors
 * 
 * Author             : StuntTeam
 * Support            : http://sourceforge.net/projects/rflink/
 * License            : This code is free for use in any open source project when this header is included.
 *                      Usage of any parts of this code in a commercial application is prohibited!
 *********************************************************************************************
 * Changelog: v1.0 initial release
 *********************************************************************************************
 * Technical information:
 * 
 * ~0.5ms high puls followed by ~1ms pause = high
 * ~1.5ms high puls followed by ~1ms pause = low
 * 
 * Message Format: 
 * 11111111 01010101 00000101 01000101 11111111 10011110
 * FF550545FF9E
 * AABCDDEEFFGG
 *
 * A = Preamble, always FF
 * B = TX type, always 5
 * C = Address (5/6/7) > low 2 bits = 1/2/3
 * D = Soil moisture 05% 
 * E = temperature 
 * F = security code, always F
 * G = Checksum 55+05+45+FF=19E CRC value = 9E
 *
 * Sample:
 * 20;02;DEBUG;Pulses=96;Pulses(uSec)=390,870,420,870,420,870,420,870,420,870,420,870,420,870,420,870,1260,870,420,870,1260,870,420,870, 1230,870,420,870,1260,870,420,870,1260,870,1260,870,1260,870,1230,870,1260,870,420,870,1260,870,420,870,1260,870,420,870,1260,870,1260,870,1260,870,420,870,1260,870,420,870,420,870,420,870,420,870,420,870,420,870,420,870,420,870,420,870,420,870,1230,870,1260,870,420,870,420,870,420,840,420,840,1260,6990;
 * 111111110101010100000101010001011111111110011110
 \*********************************************************************************************/
#define IMAGINTRONIX_PULSECOUNT 96
#define IMAGINTRONIX_PULSEMID   1000/RAWSIGNAL_SAMPLE_RATE
#define IMAGINTRONIX_PULSESHORT 550/RAWSIGNAL_SAMPLE_RATE

#ifdef PLUGIN_035
boolean Plugin_035(byte function, char *string) {
      if (RawSignal.Number != IMAGINTRONIX_PULSECOUNT) return false;
      unsigned int temperature=0;
      unsigned int rc=0;
      
      byte checksum=0;
      byte data[8];
      unsigned long bitstream=0L;
      //==================================================================================
      byte bytecounter=0;                           // used for counting the number of received bytes
      byte bitcounter=0;                            // counts number of received bits (converted from pulses) 
      // get bits
      for(byte x=1;x < IMAGINTRONIX_PULSECOUNT;x=x+2) {
         if (RawSignal.Pulses[x] > IMAGINTRONIX_PULSEMID) {     // long pulse = 0 bit
            if (x < 95) if ((RawSignal.Pulses[x+1] > IMAGINTRONIX_PULSEMID) || (RawSignal.Pulses[x+1] < IMAGINTRONIX_PULSESHORT)) return false;
            data[bytecounter] = (data[bytecounter] << 1);       // 0 bit
            bitcounter++;                           // received a bit
         } else {                                               // Short pulse = 1 bit
            if (RawSignal.Pulses[x] > IMAGINTRONIX_PULSESHORT) return false; // Short pulse too long? 
            if (x < 95) if ((RawSignal.Pulses[x+1] > IMAGINTRONIX_PULSEMID) || (RawSignal.Pulses[x+1] < IMAGINTRONIX_PULSESHORT)) return false;
            data[bytecounter] = (data[bytecounter] << 1) | 0x1; // 1 bit
            bitcounter++;                        // received a bit
         }
         // prepare for next bit/byte
         if (bitcounter==8) {                 // received 8 bits?
            bitcounter=0;                     // reset for next byte
            bytecounter++;                    // byte received, increase counter
            if (bytecounter > 7) return false; // overflow, should not happen
         }
      }
      //==================================================================================
      // Verify packet and calculate checksum
      //==================================================================================
      if (data[0] != 0xff) return false;
      checksum=data[1]+data[2]+data[3]+data[4];
      if (((checksum)&0xff) != data[5]) return false;
      if ( (data[1] >>4) != 0x5) return false;
      if (data[4] != 0xff) return false;
      //==================================================================================
      // Prevent repeating signals from showing up
      //==================================================================================
      if( (SignalHash!=SignalHashPrevious) || (RepeatingTimer+1000<millis()) ) { 
         //SignalCRC=bitstream;                       // not seen the RF packet recently
      } else {
         return true;                               // already seen the RF packet recently
      }
      //==================================================================================
      rc=(data[1])&0x03;
      temperature=(data[3]) << 4; 
      temperature=temperature/2;
      //==================================================================================
      // Output
      // ----------------------------------
      sprintf(pbuffer, "20;%02X;", PKSequenceNumber++); // Node and packet number 
      Serial.print( pbuffer );
      Serial.print(F("Imagintronix;"));                   // Label
      sprintf(pbuffer, "ID=%04x;", rc);             // ID    
      Serial.print( pbuffer );
      sprintf(pbuffer, "TEMP=%04x;", temperature);     
      Serial.print( pbuffer );
      sprintf(pbuffer, "HUM=%02x;", data[2]);     
      Serial.print( pbuffer );
      Serial.println();
      //==================================================================================
      RawSignal.Repeats=true;                       // suppress repeats of the same RF packet
      RawSignal.Number=0;
      return true;
}
#endif // PLUGIN_035
