//#######################################################################################################
//##                    This Plugin is only for use with the RFLink software package                   ##
//##                                        Plugin-30 AlectoV1                                         ##
//#######################################################################################################
/*********************************************************************************************\
 * This plugin takes care of decoding the protocol used for outdoor sensors of the Alecto weather stations 
 * following protocol version 1
 * This Plugin works at least with: Alecto WS3500, Silvercrest, Otio sht-10, Otio sht-20
 *                                  Auriol H13726, Ventus WS155, Hama EWS 1500, Meteoscan W155/W160 
 *                                  Alecto WS4500, Ventus W044, Balance RF-WS105 
 * 
 * Author             : StuntTeam
 * Support            : http://sourceforge.net/projects/rflink/
 * License            : This code is free for use in any open source project when this header is included.
 *                      Usage of any parts of this code in a commercial application is prohibited!
 *********************************************************************************************
 * Technische informatie:
 * Message Format: (9 nibbles, 36 bits):
 *
 * Format for Temperature Humidity
 *   AAAAAAAA BBBB CCCC CCCC CCCC DDDDDDDD EEEE
 *   00110110 1000 1011 0111 0000 00011100 0110
 *   RC       Type Temperature___ Humidity Checksum
 *   A = Rolling Code (includes channel number on low 2 bits of nibble1 (10=ch1 01=ch2 11=ch3) )
 *   B = Message type (xyyx = temp/humidity if yy <> '11') => only accepting yy = 00 for negative temperatures for now
 *       4 bits: bit 0   = battery state 0=OK, 1= below 2.6 volt       
 *               bit 1&2 = 00/01/10 = temp/hum is transmitted, 11=non temp is transmitted 
 *               bit 3   = 0=scheduled transmission, 1=requested transmission (button press)
 *   C = Temperature (two's complement)
 *   D = Humidity BCD format
 *   E = Checksum
 *
 * Sample: 
 * 20;F4;DEBUG;Pulses=74;Pulses(uSec)=450,1900,350,1900,350,3975,350,3975,350,1900,350,3975,350,3975,350,1900,350,3975,350,1900,350,1900,350,1900,350,3975,350,1900,350,3975,350,3975,350,1900,350,3975,350,3975,350,3975,350,1900,350,1900,350,1900,350,1900,350,1900,350,1900,350,1900,350,3975,350,3975,350,3975,350,1900,350,1900,350,1900,350,3975,350,3975,350,2025,350;
 * 20;F5;Alecto V1;ID=006c;TEMP=00ed;HUM=38;
 *
 * Format for Rain
 *   AAAAAAAA BBBB CCCC DDDD DDDD DDDD DDDD EEEE
 *   01100001 0110 1100 1010 1011 0010 0000 0010
 *   RC       Type      Rain                Checksum
 *   A = Rolling Code
 *   B = Message type (xyyx = NON temp/humidity data if yy = '11')
 *   C = fixed to 1100
 *   D = Rain (bitvalue * 0.25 mm)
 *   E = Checksum
 *
 * Sample: 
 * 20;A8;DEBUG;Pulses=74;Pulses(uSec)=550,1925,425,4100,425,4100,425,1975,425,1975,425,1975,425,1975,425,4100,400,2000,425,4100,425,4100,425,1975,425,4100,425,4100,425,1975,425,1975,425,4100,425,1975,425,4100,400,1975,425,4100,425,1975,425,4100,425,4100,425,1975,450,1975,425,4100,450,1950,450,1950,450,1950,425,1975,450,1950,450,1950,475,1925,500,4025,475,1950,475;
 * 20;A9;Alecto V1;ID=0086;RAIN=04d5;
 * 
 * Format for Windspeed
 *   AAAAAAAA BBBB CCCC CCCC CCCC DDDDDDDD EEEE
 *   RC       Type                Windspd  Checksum
 *   A = Rolling Code
 *   B = Message type (xyyx = NON temp/humidity data if yy = '11')
 *   C = Fixed to 1000 0000 0000
 *   D = Windspeed  (bitvalue * 0.2 m/s, correction for webapp = 3600/1000 * 0.2 * 100 = 72)
 *   E = Checksum
 *
 * Format for Winddirection & Windgust
 *   AAAAAAAA BBBB CCCD DDDD DDDD EEEEEEEE FFFF
     01110000 0000 1111 1011 0000 00000000 0101
 *   RC       Type      Winddir   Windgust Checksum
 *   A = Rolling Code
 *   B = Message type (xyyx = NON temp/humidity data if yy = '11')
 *   C = Fixed to 111x
 *   D = Wind direction 0-511 in 0.7 degree steps?
 *   E = Windgust (bitvalue * 0.2 m/s, correction for webapp = 3600/1000 * 0.2 * 100 = 72)
 *   F = Checksum
 *
 * Sample:
 * 20;53;DEBUG;Pulses=74;Pulses(uSec)=425,3800,350,1825,350,1825,325,1825,350,1825,325,3800,350,3800,350,1825,325,3800,350,1825,325,1800,350,1825,350,1825,325,1825,325,3800,325,1825,350,1800,350,1825,325,3825,325,3800,325,1825,325,1825,325,1800,325,1825,350,3800,325,1825,325,3800,350,1800,350,1800,350,3800,350,1825,325,1825,325,1825,325,1825,350,1825,325,1925,325;
 \*********************************************************************************************/
#define WS3500_PULSECOUNT 74

#ifdef PLUGIN_030
boolean Plugin_030(byte function, char *string) {
      if (RawSignal.Number != WS3500_PULSECOUNT) return false;
      unsigned long bitstream=0L;
      byte nibble0=0;
      byte nibble1=0;
      byte nibble2=0;
      byte nibble3=0;
      byte nibble4=0;
      byte nibble5=0;
      byte nibble6=0;
      byte nibble7=0;
      byte checksum=0;
      int temperature=0;
      byte humidity=0;
      unsigned int rain=0;
      byte windspeed=0;
      byte windgust=0;
      int winddirection=0;
      byte checksumcalc = 0;
      byte rc=0;
      byte battery=0;
      //==================================================================================
      for(byte x=2; x<=64; x=x+2) {
         if (RawSignal.Pulses[x+1]*RawSignal.Multiply > 700) return false; // in between pulses should be short
         if (RawSignal.Pulses[x]*RawSignal.Multiply > 2560) {
            bitstream = ((bitstream >> 1) |(0x1L << 31)); 
         } else {
            bitstream = (bitstream >> 1);
         }
      }
      for(byte x=66; x<=72; x=x+2) {
         if (RawSignal.Pulses[x]*RawSignal.Multiply > 2560) {
            checksum = ((checksum >> 1) |(0x1L << 3)); 
         } else {
            checksum = (checksum >> 1);
         }
      }
      //==================================================================================
      if (bitstream == 0) return false;          // Perform a sanity check
      //==================================================================================
      // Prevent repeating signals from showing up
      //==================================================================================
      if( (SignalHash!=SignalHashPrevious) || ((RepeatingTimer+1000<millis()) && (SignalCRC != bitstream)) || (SignalCRC != bitstream) ) { 
         // not seen the RF packet recently
         SignalCRC=bitstream;
      } else {
         // already seen the RF packet recently
         return true;
      }
      //==================================================================================
      // Sort nibbles
      nibble7 = (bitstream >> 28) & 0xf;
      nibble6 = (bitstream >> 24) & 0xf;
      nibble5 = (bitstream >> 20) & 0xf;
      nibble4 = (bitstream >> 16) & 0xf;
      nibble3 = (bitstream >> 12) & 0xf;
      nibble2 = (bitstream >> 8) & 0xf;
      nibble1 = (bitstream >> 4) & 0xf;
      nibble0 = bitstream & 0xf;
      //==================================================================================
      // Perform checksum calculations, Alecto checksums are Rollover Checksums by design!
      if ((nibble2 & 0x6) != 6) { // temperature packet
         checksumcalc = (0xf - nibble0 - nibble1 - nibble2 - nibble3 - nibble4 - nibble5 - nibble6 - nibble7) & 0xf;
      } else {
         if ((nibble3 & 0x7) == 3) { // Rain packet
            checksumcalc = (0x7 + nibble0 + nibble1 + nibble2 + nibble3 + nibble4 + nibble5 + nibble6 + nibble7) & 0xf;
         } else { // Wind packet
            checksumcalc = (0xf - nibble0 - nibble1 - nibble2 - nibble3 - nibble4 - nibble5 - nibble6 - nibble7) & 0xf;
         }
      }
      if (checksum != checksumcalc) return false;
      //==================================================================================
      battery=(nibble2)&0x1;                              // get battery indicator
      nibble2=(nibble2)&0x06;                             // prepare nibble to contain only the needed bits
      nibble3=(nibble3)&0x07;                             // prepare nibble to contain only the needed bits
      //==================================================================================
      rc = bitstream & 0xff;

      if ((nibble2) != 6) {  // nibble 2 needs to be set to something other than 'x11x' to be a temperature packet
         // Temperature packet
         temperature = (bitstream >> 12) & 0xfff;
         //fix 12 bit signed number conversion
         if ((temperature & 0x800) == 0x800) {
            if ((nibble2 & 0x6) != 0) return false;       // reject alecto v4 on alecto v1... (causing high negative temperatures with valid checksums)
            temperature=4096-temperature;                 // fix for minus temperatures
            if (temperature > 0x258) return false;        // temperature out of range ( > -60.0 degrees) 
            temperature=temperature | 0x8000;             // turn highest bit on for minus values
         } else {
            if (temperature > 0x258) return false;        // temperature out of range ( > 60.0 degrees) 
         }
         humidity = (16 * nibble7) + nibble6;
         if (humidity > 0x99) return false;               // Humidity out of range, assume ALL data is bad?
         //==================================================================================
         // Output
         // ----------------------------------
         sprintf(pbuffer, "20;%02X;", PKSequenceNumber++); // Node and packet number 
         Serial.print( pbuffer );
         // ----------------------------------
         Serial.print(F("Alecto V1;"));                  // Label
         sprintf(pbuffer, "ID=%02x%02x;", (rc &0x03), (rc &0xfc)  ); // ID is split into channel number and rolling code 
         Serial.print( pbuffer );
         sprintf(pbuffer, "TEMP=%04x;", temperature);     
         Serial.print( pbuffer );
         if (humidity < 0x99) {                          // Some AlectoV1 devices actually lack the humidity sensor and always report 99%
            sprintf(pbuffer, "HUM=%02x;", humidity);     // Only report humidity when it is below 99%
            Serial.print( pbuffer );                     
         }
         if (battery==0) {          
            Serial.print("BAT=OK;");
         } else {
            Serial.print("BAT=LOW;");
         }
         Serial.println();
         //==================================================================================
         RawSignal.Repeats=true;                          // suppress repeats of the same RF packet
         RawSignal.Number=0;
         return true;
      } else {
         if ((nibble3) == 3) { // Rain packet
            rain = ((bitstream >> 16) & 0xffff);
            //==================================================================================
            // Output
            // ----------------------------------
            sprintf(pbuffer, "20;%02X;", PKSequenceNumber++); // Node and packet number 
            Serial.print( pbuffer );
            // ----------------------------------
            Serial.print(F("Alecto V1;"));                 // Label
            sprintf(pbuffer, "ID=00%02x;", rc);             // ID    
            Serial.print( pbuffer );
            sprintf(pbuffer, "RAIN=%04x;", rain);     
            Serial.print( pbuffer );
            Serial.println();
            //==================================================================================
            RawSignal.Repeats=true;                        // suppress repeats of the same RF packet
            RawSignal.Number=0;
            return true;
         }
         if ((nibble3) == 1) {  // windspeed packet
            windspeed = ((bitstream >> 24) & 0xff);
            windspeed = windspeed*72;
            //==================================================================================
            // Output
            // ----------------------------------
            sprintf(pbuffer, "20;%02X;", PKSequenceNumber++); // Node and packet number 
            Serial.print( pbuffer );
            // ----------------------------------
            Serial.print(F("Alecto V1;"));             // Label
            sprintf(pbuffer, "ID=00%02x;", rc);         // ID    
            Serial.print( pbuffer );
            sprintf(pbuffer, "WINSP=%04x;", windspeed);     
            Serial.print( pbuffer );
            Serial.println();
            //==================================================================================
            RawSignal.Repeats=true;                    // suppress repeats of the same RF packet
            RawSignal.Number=0;
            return true;
         }
         if ((nibble3) == 7) { // winddir packet
            winddirection = ((bitstream >> 15) & 0x1ff) / 45; // ???
            winddirection = winddirection & 0x0f; 
            windgust = ((bitstream >> 24) & 0xff);
            windgust = windgust*72;
            //==================================================================================
            // Output
            // ----------------------------------
            sprintf(pbuffer, "20;%02X;", PKSequenceNumber++); // Node and packet number 
            Serial.print( pbuffer );
            // ----------------------------------
            Serial.print(F("Alecto V1;"));             // Label
            sprintf(pbuffer, "ID=00%02x;", rc);         // ID    
            Serial.print( pbuffer );
            sprintf(pbuffer, "WINDIR=%04d;", winddirection);     
            Serial.print( pbuffer );
            sprintf(pbuffer, "WINGS=%04x;", windgust);     
            Serial.print( pbuffer );
            Serial.println();
            //==================================================================================
            RawSignal.Repeats=true;                    // suppress repeats of the same RF packet
            RawSignal.Number=0;
            return true;
         }
      }
      return false;
}
#endif // PLUGIN_030
