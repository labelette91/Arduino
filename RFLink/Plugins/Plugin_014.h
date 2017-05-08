//#######################################################################################################
//##                    This Plugin is only for use with the RFLink software package                   ##
//##                                        Plugin-14: Ikea Koppla                                     ##
//#######################################################################################################
/*********************************************************************************************\
 * This plugin takes care of sending and receiving the Ikea Koppla protocol
 * 
 * Author             : StuntTeam
 * Support            : http://sourceforge.net/projects/rflink/
 * License            : This code is free for use in any open source project when this header is included.
 *                      Usage of any parts of this code in a commercial application is prohibited!
 ***********************************************************************************************
 * Technical information:
 * Packets are variable length as zero bits are 2 pulses and 1 bits are single pulses.
 * Pulse lengths are ~1600 for a 0 bit and two times ~750 for a 1 bit
 * Packets contain 28 bits
 *
 * AAAA BBBB CCCCCCCCCC DD EEEEEE FF
 * 1110 0011 0000000001 01 000010 01 on
 * 1110 0011 0000000001 01 010110 01 off
 * 1110 1111 1111111111 00 001110 10 Pair
 *
 * A = Preamble, Always '1110'
 * B = System code 0-0x0f
 * C = Unit code bit selection, order: ch10,1,2,3,4,5,6,7,8,9 
 * D = Checksum on B
 * E = Level and Fade 
 * F = Checksum on D 
 *
 * Sample:
 * 20;07;DEBUG;Pulses=38;Pulses(uSec)=825,775,750,775,750,775,1600,1625,1600,775,750,775,750,1625,1600,1625,1600,1625,1625,1625,1600,1625,750,750,1600,775,750,1625,1600,1625,1600,775,750,1625,1600,775,750;
 * 20;08;DEBUG;Pulses=40;Pulses(uSec)=925,775,750,775,750,775,1600,1625,1600,775,750,775,750,1625,1600,1625,1600,1625,1625,1625,1600,1625,750,775,1600,775,750,1625,750,750,1625,775,750,775,750,1625,1600,750,750;
 \*********************************************************************************************/
#define KOPPLA_PulseLength_MIN  36
#define KOPPLA_PulseLength_MAX  52
#define KOPPLA_PULSEMID         1300/RAWSIGNAL_SAMPLE_RATE
#define KOPPLA_PULSEMAX         1850/RAWSIGNAL_SAMPLE_RATE
#define KOPPLA_PULSEMIN         650/RAWSIGNAL_SAMPLE_RATE

#ifdef PLUGIN_014
boolean Plugin_014(byte function, char *string) {
   if ((RawSignal.Number < KOPPLA_PulseLength_MIN) || (RawSignal.Number > KOPPLA_PulseLength_MAX) ) return false; 
      unsigned long bitstream=0L;
      
      byte preamble=0;
      unsigned int sysunit=0;
      byte levelfade=0;
      byte command=0;
      byte level=0;
      int i=0;

      byte checksum1=0;
      byte checksum2=0;
      byte checksum=0x03;
      byte tempcrc=0;
      // ==========================================================================
      for (byte x=1;x < RawSignal.Number;x++) {  
          if (RawSignal.Pulses[x] > KOPPLA_PULSEMID) {                  // long pulse, 0 bit
             if (RawSignal.Pulses[x] > KOPPLA_PULSEMAX) return false;   // long pulse is too long
             bitstream = (bitstream << 1);                              // 0 bit
          } else {                                                      // Short pulse
             if (RawSignal.Pulses[x] < KOPPLA_PULSEMIN) return false;   // short pulse is too short
             if (RawSignal.Pulses[x+1] > KOPPLA_PULSEMID) return false; // need 2 short pulses for a 1 bit
             if (RawSignal.Pulses[x+1] < KOPPLA_PULSEMIN) return false; // second short pulse is too short
             x++;                                                       // skip second short pulse
             bitstream = (bitstream << 1) | 0x1;                        // 1 bit
          }
      }
      //==================================================================================
      // Sort data and perform sanity checks
      //==================================================================================
      if (bitstream==0) return false;               // No bits detected? 
      if ((((bitstream) >> 24)&0x0f) != 0x0e) return false; // Preamble should always be '1110'
      sysunit=(((bitstream) >> 10)& 0x03fff);       // 4 bits system code and 10 bits unit code
      checksum1=(((bitstream) >> 8) &0x03);         // first checksum
      levelfade=(((bitstream) >> 2)& 0x3f);         // 6 bits level and face code
      checksum2=((bitstream)&0x03);                 // second checksum
      // calculate checksum 1
      checksum=3;
      for (i=0;i<7;i++) {
          tempcrc=((sysunit) >> (i*2)) & 3;
          checksum=checksum ^ tempcrc;
      }
      if (checksum1 != checksum) return false;
      // calculate checksum 2
      checksum=3;
      for (i=0;i<3;i++) {
          tempcrc=((levelfade) >> (i*2)) & 3;
          checksum=checksum ^ tempcrc;
      }
      if (checksum2 != checksum) return false;
      // sort command / dim bits
      if ((levelfade) == 0x2) { // on
         command=1;
      } else
      if ((levelfade) == 0x16) { // off
         command=0;
      } else
      if ((levelfade) == 0x0) {  // up
         command=2;
      } else
      if ((levelfade) == 0x4) {  // down
         command=3;
      } else {
         command=4;
         for (i=2;i<6;i++) {
             level=level<<1;
             level=level | ((levelfade)>>i)&0x01;
         }
      }
      //==================================================================================
      // ----------------------------------
      // Output
      // ----------------------------------
      sprintf(pbuffer, "20;%02X;", PKSequenceNumber++); // Node and packet number 
      Serial.print( pbuffer );
      // ----------------------------------
      Serial.print(F("Ikea Koppla;"));                           // Label
      sprintf(pbuffer, "ID=%04x;", sysunit); // ID    
      Serial.print( pbuffer );
      sprintf(pbuffer, "SWITCH=%02x;", levelfade); // ID    
      Serial.print( pbuffer );
      Serial.print(F("CMD="));                    

      if ( command == 0) {
         Serial.print(F("OFF;"));
      } else 
      if ( command == 1) {
         Serial.print(F("ON;"));
      } else 
      if ( command == 2) {
         Serial.print(F("BRIGHT;"));
      } else 
      if ( command == 3) {
         Serial.print(F("DIM;"));
      } else {
         sprintf(pbuffer, "SET_LEVEL=%d;", level );     
         Serial.print( pbuffer );
      }
      Serial.println();
      // ----------------------------------
      RawSignal.Repeats=true;                    // suppress repeats of the same RF packet         
      RawSignal.Number=0;
      return true;
}
#endif //PLUGIN_014
  
