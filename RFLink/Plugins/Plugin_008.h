//#######################################################################################################
//##                    This Plugin is only for use with the RFLink software package                   ##
//##                                      Plugin-008 Kambrook                                          ##
//#######################################################################################################
/*********************************************************************************************\
 * This Plugin takes care of receiving of the Kambrook protocol
 * Device models:  RF3399/RF3405/RF3672/RF3689/RF4471R 
 * Made by Ningbo Comen Electronics Technology Co. Ltd. 
 *
 * Author             : StuntTeam
 * Support            : http://sourceforge.net/projects/rflink/
 * License            : This code is free for use in any open source project when this header is included.
 *                      Usage of any parts of this code in a commercial application is prohibited!
 *********************************************************************************************
 * Changelog: v1.0 initial release
 *********************************************************************************************
 * Technical information:            
 * RF packets contain 96 pulses, 48 bits
 *
 * Kambrook Message Format: 
 * AAAAAAAA BBBBBBBB BBBBBBBB BBBBBBBB CCCCCCCC DDDDDDDD
 *
 * A = Preamble, always 0x55
 * B = Address
 * C = Channel/Command
 * D = Trailing, always 0xFF
 *  
 * Details http://wiki.beyondlogic.org/index.php?title=Reverse_engineering_the_RF_protocol_on_a_Kambrook_Power_Point_Controller
 *
 * 20;33;DEBUG;Pulses=96;Pulses(uSec)=270,180,600,180,210,180,600,180,210,180,600,180,210,180,600,180,210,180,210,180,210,180,210,180,210,180,210,180,210,180,210,180,210,180,210,180,210,180,210,180,210,180,210,180,210,180,210,180,210,180,210,180,210,180,210,180,210,180,210,180,210,180,600,180,210,180,210,180,210,180,210,180,210,180,210,180,210,180,600,180,600,180,600,180,600,180,600,180,600,180,600,180,600,180,600,6990;
 \*********************************************************************************************/
#define KAMBROOK_PULSECOUNT 96
#define KAMBROOK_PULSEMID  400/RAWSIGNAL_SAMPLE_RATE

#ifdef PLUGIN_008
boolean Plugin_008(byte function, char *string) {
      if (RawSignal.Number != KAMBROOK_PULSECOUNT) return false;
      unsigned long address=0L;
      byte sync=0;
      byte command=0;
      byte trailing=0;
      
      byte bitcounter=0;
      byte status=0;
      byte channel=0;
      byte subchan=0;
      //==================================================================================
      // get bits
      for(byte x=1;x<KAMBROOK_PULSECOUNT;x=x+2) {
         if (RawSignal.Pulses[x] > KAMBROOK_PULSEMID) {
            if (bitcounter < 8) {
                sync = (sync << 1) | 0x1;   
            } else 
            if (bitcounter < 32) {
                address = (address << 1) | 0x1;  
            } else 
            if (bitcounter < 40) {
                command = (command << 1) | 0x1;  
            } else {
                trailing = (trailing << 1) | 0x1;  
            }
            bitcounter++;
         } else {
            if (bitcounter < 8) {
                sync = (sync << 1);   
            } else 
            if (bitcounter < 32) {
                address = (address << 1);  
            } else 
            if (bitcounter < 40) {
                command = (command << 1);  
            } else {
                trailing = (trailing << 1);  
            }
            bitcounter++;
         }
      }
      //==================================================================================
      // all bits received, make sure checksum/sanity check is okay
      //==================================================================================
      if (sync != 0x55) return false;
      if (trailing != 0xff) return false;
      //==================================================================================
      // Prevent repeating signals from showing up
      //==================================================================================
      if(SignalHash!=SignalHashPrevious || RepeatingTimer<millis()) { 
         // not seen the RF packet recently
      } else {
         // already seen the RF packet recently
         return true;
      }      
      //==================================================================================
      //==================================================================================
      status=(command)&1;                   // 0/1 off/on

      subchan=(((command) >> 1)&7) + 1;     // button code
      if (status == 0) subchan--;

      byte temp=(command) >> 4;             // channel code
      channel=0x41+temp;
      //==================================================================================
      // Output
      // ----------------------------------
      sprintf(pbuffer, "20;%02X;", PKSequenceNumber++);// Node and packet number 
      Serial.print( pbuffer );
      // ----------------------------------
      Serial.print(F("Kambrook;"));                 // Label
      sprintf(pbuffer, "ID=%06lx;",((address)&0xffffff) );   // ID   
      Serial.print( pbuffer );
      sprintf(pbuffer, "SWITCH=%c%d;", channel,subchan);     
      Serial.print( pbuffer );
      Serial.print(F("CMD="));                      // command
      if (status==0) Serial.print(F("OFF;")); 
      if (status==1) Serial.print(F("ON;"));
      Serial.println();
      //==================================================================================
      RawSignal.Repeats=true;                    // suppress repeats of the same RF packet         
      RawSignal.Number=0;
      return true;
}
#endif // PLUGIN_008

#ifdef PLUGIN_TX_008
void Kambrook_Send(unsigned long address);

boolean PluginTX_008(byte function, char *string) {
        boolean success=false;
        //10;kambrook;050325;a1;ON;
        //012345678901234567890123
        if (strncasecmp(InputBuffer_Serial+3,"KAMBROOK;",9) == 0) { // KAKU Command eg. 
           if (InputBuffer_Serial[18] != ';') return false;
          
           InputBuffer_Serial[10]=0x30;
           InputBuffer_Serial[11]=0x78;                            // Get address from hexadecimal value 
           InputBuffer_Serial[18]=0x00;                            // Get address from hexadecimal value 

           unsigned long bitstream=0L;              // Main placeholder
           byte Home=0;                             // channel A..D
           byte Address=0;                          // subchannel 1..5
           byte c;
           byte x=19;                               // teller die wijst naar het te behandelen teken

           bitstream=str2int(InputBuffer_Serial+10);               // Address

           while((c=tolower(InputBuffer_Serial[x++]))!=';') {
                 if(c>='0' && c<='9'){Address=Address+c-'0';}   // Home 0..9
                 if(c>='a' && c<='d'){Home=c-'a';}     // Address a..d
           }

           Address=Address << 1;                 // shift left 1 bit 
           c=0;
           c = str2cmd(InputBuffer_Serial+x);                      // ON/OFF command
           if (c == VALUE_ON) { 
              Address--;
           }
           Address=Address & 0x0f;
           // -------------------------------
           Home = Home << 4;                        // move home to bits 4-7
           bitstream=(bitstream) << 8;              // shift main value 8 bits left 
           bitstream=bitstream + Home;              // add to main value
           bitstream=bitstream + Address;           // add address bits to bits 0-3
           // -------------------------------
           Kambrook_Send(bitstream);                // bitstream to send
           success=true;
        }
        return success;
}

void Kambrook_Send(unsigned long address) { 
    int fpulse = 300;                                  // Pulse witdh in microseconds
    int fpulse2 = 700;                                 // Pulse witdh in microseconds
    int fretrans = 5;                                  // Number of code retransmissions
    uint32_t fdatabit;
    uint32_t fdatamask = 0x800000;
    uint32_t fsendbuff;

    digitalWrite(PIN_RF_RX_VCC,LOW);                   // Spanning naar de RF ontvanger uit om interferentie met de zender te voorkomen.
    digitalWrite(PIN_RF_TX_VCC,HIGH);                  // zet de 433Mhz zender aan
    delayMicroseconds(TRANSMITTER_STABLE_DELAY);       // short delay to let the transmitter become stable (Note: Aurel RTX MID needs 500µS/0,5ms)
    for (int nRepeat = 0; nRepeat <= fretrans; nRepeat++) {
        // --------------
        // Send preamble (0x55) - 8 bits
        fsendbuff=0x55;
        fdatamask=0x80;
        for (int i = 0; i < 8; i++) {                  // Preamble
            // read data bit
            fdatabit = fsendbuff & fdatamask;          // Get most left bit
            fsendbuff = (fsendbuff << 1);              // Shift left
            if (fdatabit != fdatamask) { // Write 0
                digitalWrite(PIN_RF_TX_DATA, HIGH);
                delayMicroseconds(fpulse);
                digitalWrite(PIN_RF_TX_DATA, LOW);
                delayMicroseconds(fpulse);
            } else { // Write 1
                digitalWrite(PIN_RF_TX_DATA, HIGH);
                delayMicroseconds(fpulse2);
                digitalWrite(PIN_RF_TX_DATA, LOW);
                delayMicroseconds(fpulse);
            }
        }
        // --------------
        fsendbuff=address;
        fdatamask=0x80000000;
        // Send command (channel/address/status) - 32 bits
        for (int i = 0; i < 32;i++){ //28;i++){
            // read data bit
            fdatabit = fsendbuff & fdatamask;          // Get most left bit
            fsendbuff = (fsendbuff << 1);              // Shift left
            if (fdatabit != fdatamask) { // Write 0
                digitalWrite(PIN_RF_TX_DATA, HIGH);
                delayMicroseconds(fpulse);
                digitalWrite(PIN_RF_TX_DATA, LOW);
                delayMicroseconds(fpulse);
            } else { // Write 1
                digitalWrite(PIN_RF_TX_DATA, HIGH);
                delayMicroseconds(fpulse2);
                digitalWrite(PIN_RF_TX_DATA, LOW);
                delayMicroseconds(fpulse);
            }
        }
        // --------------
        // Send trailing bits - 8 bits
        fsendbuff=0xFF;
        fdatamask=0x80;
        for (int i = 0; i < 8; i++) {    
            // read data bit
            fdatabit = fsendbuff & fdatamask;          // Get most left bit
            fsendbuff = (fsendbuff << 1);              // Shift left
            if (fdatabit != fdatamask) { // Write 0
                digitalWrite(PIN_RF_TX_DATA, HIGH);
                delayMicroseconds(fpulse);
                digitalWrite(PIN_RF_TX_DATA, LOW);
                delayMicroseconds(fpulse);
            } else { // Write 1
                digitalWrite(PIN_RF_TX_DATA, HIGH);
                delayMicroseconds(fpulse2);
                digitalWrite(PIN_RF_TX_DATA, LOW);
                delayMicroseconds(fpulse);
            }
        }
        // --------------
        digitalWrite(PIN_RF_TX_DATA, LOW);
        delayMicroseconds(fpulse2 * 14); 
    }
    delayMicroseconds(TRANSMITTER_STABLE_DELAY);       // short delay to let the transmitter become stable (Note: Aurel RTX MID needs 500µS/0,5ms)
    digitalWrite(PIN_RF_TX_VCC,LOW);                   // zet de 433Mhz zender weer uit
    digitalWrite(PIN_RF_RX_VCC,HIGH);                  // Spanning naar de RF ontvanger weer aan.
    RFLinkHW();
}
#endif // PLUGIN_008
