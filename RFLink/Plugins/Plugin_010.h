
//##                    This Plugin is only for use with the RFLink software package                   ##
//##                                      Plugin-10 TRC02 RGB Controller                               ##
//#######################################################################################################
/*********************************************************************************************\
 * Decodes signals from a wireless RGB TRC02 controller remote control
 * 
 *
 * Author             : StuntTeam, Marek Zlatos,
 * Support            : http://sourceforge.net/projects/rflink/
 * License            : This code is free for use in any open source project when this header is included.
 *                      Usage of any parts of this code in a commercial application is prohibited!
 *********************************************************************************************
 * Changelog: v1.0 initial release
 *********************************************************************************************
 * Technical information:
 * Decodes signals from a wireless TRC02 RGB controller remote control
 * --------------------------------------------------------------------------------------------
 * _Byte 0_  _Byte 1_  _Byte 2_  _Byte 3_  _Bit_
 * 76543210  76543210  76543210  76543210  0
 * AAAAAAAA  BBBBBBBB  CCCCCCCC  DDDDDDDD  E
 *
 * A = Rolling Code
 * B = Rolling Code
 * C = Rolling Code
 * D = Command
 * E = Checksum. bit is XOR of all bits in the RF message
 * 
 * Commands:
 * 00 ON
 * 01 OFF
 * 02 Dim Down
 * 03 DIM UP
 * 05 Color Mix UP
 * 04 Color Mix Down
 * 87 Color Wheel Red
 * 34 Color Wheel Blue
 * 78 Color Wheel Yellow
 * 5D Color Wheel Green 
 *
 * Sample:
 * 20;30;DEBUG;Pulses=180;Pulses(uSec)=450,420,420,420,420,420,1410,960,420,420,420,420,420,420,420,420,420,420,930,420,420,960,420,420,420,420,420,420,420,420,420,420,930,960,420,420,420,420,930,420,420,420,420,420,420,960,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,930,390,1440,960,420,420,420,420,420,420,420,420,420,420,930,420,420,960,420,420,420,420,420,420,420,420,420,420,930,960,420,420,420,420,930,420,420,420,420,420,420,960,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,930,390,1440,960,420,420,420,420,420,420,420,420,420,420,930,420,420,960,420,420,420,420,420,420,420,420,420,420,930,960,420,420,420,420,930,420,420,420,420,420,420,960,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,420,930,6990
 * TRC02:00000011 00000010 00111100 00000000 1 
 * 20;01;TRC02;ID=03023c;SWITCH=00;CMD=ON;
 \*********************************************************************************************/
#define RGB_MIN_PULSECOUNT  180
#define RGB_MAX_PULSECOUNT  186

#define RGB_PULSE_STHI      1600/RAWSIGNAL_SAMPLE_RATE
#define RGB_PULSE_STLO      1300/RAWSIGNAL_SAMPLE_RATE
#define RGB_PULSE_HIHI      1100/RAWSIGNAL_SAMPLE_RATE
#define RGB_PULSE_HILO      900/RAWSIGNAL_SAMPLE_RATE
#define RGB_PULSE_LOHI      600/RAWSIGNAL_SAMPLE_RATE
#define RGB_PULSE_LOLO      400/RAWSIGNAL_SAMPLE_RATE

#ifdef PLUGIN_010
boolean Plugin_010(byte function, char *string) {
      if (RawSignal.Number < RGB_MIN_PULSECOUNT || RawSignal.Number > RGB_MAX_PULSECOUNT) return false; 
      unsigned long bitstream=0L;                   // holds first 32 bits 

      byte checksum=0;                              // holds the checksum calculation
      byte crc=0;                                   // holds the crc bit from the signal
      byte bitcounter=0;                            // counts number of received bits (converted from pulses)
      byte halfbit=0;                               // high pulse = 1, 2 low pulses = 0, halfbit keeps track of low pulses
	  byte bitmask = 0;
      int command=0;
      byte start_stop=0;
      byte x=1;
      //==================================================================================
      for(x=1;x <RawSignal.Number-2;x++) {          // get bytes
         if (start_stop!=0x01) { 
            //if (RawSignal.Pulses[x]*RawSignal.Multiply > 1200 && RawSignal.Pulses[x]*RawSignal.Multiply < 1500) {
            if (RawSignal.Pulses[x] > RGB_PULSE_STLO && RawSignal.Pulses[x] < RGB_PULSE_STHI) {
               start_stop=0x01;
               continue;
            } else {
               if (x > 100) return false;           // bad packet
               continue;
            } 
         }
         if (RawSignal.Pulses[x]*RawSignal.Multiply > 750 && RawSignal.Pulses[x]*RawSignal.Multiply < 1000) {
            if (halfbit==1) {                       // cant receive a 1 bit after a single low value
               return false;                        // pulse error, must not be a UPM packet or reception error
            }
			bitmask = !bitmask;
            if (bitcounter < 32) {
				if (bitmask == 1){
					bitstream = (bitstream << 1);
				} else {
					bitstream = (bitstream << 1) | 0x1;
				}
                bitcounter++;                       // only need to count the first 10 bits
            } else {
                crc =1;
                break;
            }
            halfbit=0;                              // wait for next first low or high pulse     
         } else {
            if (RawSignal.Pulses[x]*RawSignal.Multiply > 625 && RawSignal.Pulses[x]*RawSignal.Multiply < 250) return false; // Not a valid UPM pulse length
            if (halfbit == 0) {                     // 2 times a low value = 0 bit
               halfbit=1;                           // first half received   
            } else {
               if (bitcounter < 32) {
                   if (bitmask == 1){
						bitstream = (bitstream << 1);
				   } else {
						bitstream = (bitstream << 1) | 0x1;
				   }
                   checksum=checksum^1;
                   bitcounter++;                     // only need to count the first 10 bits
               } else {
                  crc=0;
                  break;
               }
               halfbit=0;                           // wait for next first low or high pulse     
            }
         }
      }
      //==================================================================================
      // Validity checks
      if (RawSignal.Pulses[x+2]*RawSignal.Multiply < 1200 || RawSignal.Pulses[x+2]*RawSignal.Multiply > 1500) return false;
      //==================================================================================
      // perform a checksum check to make sure the packet is a valid RGB control packet
      // Checksum: xor all odd and all even bits should match the last bit
      // ----------------------------------
      if (checksum != crc) {
         //Serial.println("crc2 error");
         return false;  
      }
      //==================================================================================
      // now process the command      
      //==================================================================================
      command = (bitstream) &0xff;                  // command
	  int id3 = (bitstream >> 8) &0xff;
      bitstream = (bitstream >> 16) &0xffff;        // rolling code
      //==================================================================================
      // Output
      // ----------------------------------
      sprintf(pbuffer, "20;%02X;", PKSequenceNumber++); // Node and packet number 
      Serial.print( pbuffer );
      Serial.print(F("TRC02RGB;"));                 // Label
      sprintf(pbuffer, "ID=%04x", bitstream);       // ID    
      Serial.print( pbuffer );
	  sprintf(pbuffer, "%02x;", id3);     
      Serial.print( pbuffer );
      sprintf(pbuffer, "SWITCH=%02x;", command);     
      Serial.print( pbuffer );
      Serial.print(F("CMD="));
      if (command==0x00) Serial.print("ON;");
      else if (command==0x01) Serial.print(F("OFF;"));
      else if (command==0x02) Serial.print(F("DIM DOWN;"));
      else if (command==0x03) Serial.print(F("DIM UP;"));
      else if (command==0x05) Serial.print(F("COLORMIX UP;"));
      else if (command==0x04) Serial.print(F("COLORMIX DOWN;"));
      else if (command==0x87) Serial.print(F("COLOR RED;"));
      else if (command==0x34) Serial.print(F("COLOR BLUE;"));
      else if (command==0x78) Serial.print(F("COLOR YELLOW;"));
      else if (command==0x5D) Serial.print(F("COLOR GREEN;"));
      else {
        sprintf(pbuffer, "SET_LEVEL=%d;", command ); 
        Serial.print( pbuffer );        
      }
      Serial.println();
      //==================================================================================
      RawSignal.Repeats=true;                       // suppress repeats of the same RF packet
      RawSignal.Number=0;
      return true;
}
#endif // PLUGIN_010

#ifdef PLUGIN_TX_010
void TRC02_Send(unsigned long address, int command);

boolean PluginTX_010(byte function, char *string) {
		//10;TRC02RGB;03023c;00;
        //012345678901234567890123456
        boolean success=false;
        if (strncasecmp(InputBuffer_Serial+3,"TRC02RGB;",9) == 0) { // KAKU Command eg.     
		   TRC02_Send(strtoul(InputBuffer_Serial+12,NULL,16),strtoul(InputBuffer_Serial+19,NULL,16));
           success=true;
        }
        return success;
}
          
void TRC02_Send(unsigned long address, int command) { 
	int fpulse = 500;
    int fretrans = 2;          								// Number of code retransmissions
	byte crc = 0;
    uint32_t fdatabit;
    uint32_t fdatamask = 0x80000000;
    uint32_t fsendbuff;

    digitalWrite(PIN_RF_RX_VCC,LOW);                // Turn off power to the RF receiver 
    digitalWrite(PIN_RF_TX_VCC,HIGH);               // Enable the 433Mhz transmitter
    delayMicroseconds(TRANSMITTER_STABLE_DELAY);    // short delay to let the transmitter become stable (Note: Aurel RTX MID needs 500µS/0,5ms)

    for (int nRepeat = 0; nRepeat <= fretrans; nRepeat++) {
		crc=0;
        fsendbuff=address;
		fsendbuff=(fsendbuff<<8)+command;
		digitalWrite(PIN_RF_TX_DATA, HIGH);			// start pulse
		delayMicroseconds(fpulse * 3);
		digitalWrite(PIN_RF_TX_DATA, LOW);
		delayMicroseconds(fpulse);
        for (int i = 0; i < 32; i++) {              // TRC02 packet is 32 bits + 1 bit crc
            // read data bit
            fdatabit = fsendbuff & fdatamask;       // Get most left bit
            fsendbuff = (fsendbuff << 1);           // Shift left
            if (fdatabit != fdatamask) {            // Write 0
				digitalWrite(PIN_RF_TX_DATA, LOW); 
				delayMicroseconds(fpulse);
				digitalWrite(PIN_RF_TX_DATA, HIGH);         
				delayMicroseconds(fpulse);
				crc += crc^0;
            } else {                                // Write 1
				digitalWrite(PIN_RF_TX_DATA, HIGH);
				delayMicroseconds(fpulse);
				digitalWrite(PIN_RF_TX_DATA, LOW);         
				delayMicroseconds(fpulse);
				crc += crc^1;
            }			
        }
		if (crc==1) {   							// crc pulse
			delayMicroseconds(fpulse);
			digitalWrite(PIN_RF_TX_DATA, LOW); 
			delayMicroseconds(fpulse);
			digitalWrite(PIN_RF_TX_DATA, HIGH);         
			delayMicroseconds(fpulse);
        } else {                   
			delayMicroseconds(fpulse);
			digitalWrite(PIN_RF_TX_DATA, HIGH);
			delayMicroseconds(fpulse);
			digitalWrite(PIN_RF_TX_DATA, LOW);         
			delayMicroseconds(fpulse);
        }	
    }
    delayMicroseconds(TRANSMITTER_STABLE_DELAY);    // short delay to let the transmitter become stable (Note: Aurel RTX MID needs 500µS/0,5ms)
    digitalWrite(PIN_RF_TX_VCC,LOW);                // Turn thew 433Mhz transmitter off
    digitalWrite(PIN_RF_RX_VCC,HIGH);               // Turn the 433Mhz receiver on
    RFLinkHW();
}
#endif //PLUGIN_TX_010
