//#######################################################################################################
//##                    This Plugin is only for use with the RFLink software package                   ##
//##                                      Plugin-062 Chuango AlarmSensors                              ##
//#######################################################################################################
/*********************************************************************************************\
 * This protocol provides support for Chuango Alarm sensors (Motion detectors and door/window contacts)
 * Note that these modules are reported as X10 switches to the Home Automation software so that they work correctly 
 *
 * Author             : StuntTeam
 * Support            : http://sourceforge.net/projects/rflink/
 * License            : This code is free for use in any open source project when this header is included.
 *                      Usage of any parts of this code in a commercial application is prohibited!
 *********************************************************************************************
 * Technical data:
 * Devices send 50 pulses, 24 bits
 *
 * Sample:
 * 20;03;DEBUG;Pulses=50;Pulses(uSec)=1725,325,1650,325,1625,350,575,1400,1600,400,1525,475,450,1550,1475,500,1500,500,450,1475,550,1450,
 * 550,1425,1600,400,525,1475,1500,450,1500,525,1475,500,425,1525,475,1475,500,1450,525,1450,1575,400,1550,425,1500,500,400; 
 * 20;03;DEBUG;Pulses=50;Pulses(uSec)=1620,420,1530,450,1560,390,510,1440,1560,420,1530,420,450,1530,1440,510,1440,540,360,1500,450,1470,480,1470,1560,390,510,1440,1530,420,1500,480,1470,570,330,1590,390,1530,450,1500,480,1470,1530,420,1530,420,1530,420,450,3360;
 * 101010011010011010010101100110101001010101101010 0
 * 00010010 01110100 01111000   = 0x 12 74 78
 \*********************************************************************************************/
#define ALARMPIRV1_PULSECOUNT 50

#define ALARMPIRV1_PULSEMID  700/RAWSIGNAL_SAMPLE_RATE
#define ALARMPIRV1_PULSEMAX  2000/RAWSIGNAL_SAMPLE_RATE
#define ALARMPIRV1_PULSEMIN  150/RAWSIGNAL_SAMPLE_RATE

#ifdef PLUGIN_062
boolean Plugin_062(byte function, char *string) {
      if (RawSignal.Number != ALARMPIRV1_PULSECOUNT) return false;
      if (RawSignal.Pulses[0]==63) return false;    // No need to test, packet for plugin 63
      unsigned long bitstream=0L;
      //==================================================================================
      for(byte x=2;x<=48;x=x+2) {
        if (RawSignal.Pulses[x] > ALARMPIRV1_PULSEMID) {
           if (RawSignal.Pulses[x] > ALARMPIRV1_PULSEMAX) return false;   // pulse too long
           if (RawSignal.Pulses[x-1] > ALARMPIRV1_PULSEMID) return false; // invalid pulse sequence 10/01
           bitstream = (bitstream << 1) | 0x1; 
        } else { 
           if (RawSignal.Pulses[x] < ALARMPIRV1_PULSEMIN) return false;   // pulse too short
           if (RawSignal.Pulses[x-1] < ALARMPIRV1_PULSEMID) return false; // invalid pulse sequence 10/01
           bitstream = bitstream << 1;
        }
      }
      //==================================================================================
      // Prevent repeating signals from showing up
      //==================================================================================
      if( (SignalHash!=SignalHashPrevious) || (RepeatingTimer+2000<millis()) ){ 
         // not seen the RF packet recently
         if (bitstream == 0) return false;
      } else {
         // already seen the RF packet recently
         return true;
      }       
      //==================================================================================
      // Validity checks
      //==================================================================================
      // Output
      // ----------------------------------
      sprintf(pbuffer, "20;%02X;", PKSequenceNumber++); // Node and packet number 
      Serial.print( pbuffer );
      // ----------------------------------
      Serial.print(F("Chuango;"));                      // Label
      sprintf(pbuffer, "ID=%06lx;",(bitstream)&0xffffff) ; // ID    
      Serial.print( pbuffer );
      Serial.print(F("SWITCH=02;"));
      Serial.print(F("CMD=ON;"));                   // this device reports movement only
      Serial.println();
      //==================================================================================
      RawSignal.Repeats=true;                       // suppress repeats of the same RF packet
      RawSignal.Number=0;
      return true;
}
#endif // Plugin_062
