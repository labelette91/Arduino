#include "RFM69registers.h"

const char * RegName[] = {
"FIFO         ",
"OPMODE       ",
"DATAMODUL    ",
"BITRATEMSB   ",
"BITRATELSB   ",
"FDEVMSB      ",
"FDEVLSB      ",
"FRFMSB       ",
"FRFMID       ",
"FRFLSB       ",
"OSC1         ",
"AFCCTRL      ",
"LOWBAT       ",
"LISTEN1      ",
"LISTEN2      ",
"LISTEN3      ",
"VERSION      ",
"PALEVEL      ",
"PARAMP       ",
"OCP          ",
"AGCREF       ",
"AGCTHRESH1   ",
"AGCTHRESH2   ",
"AGCTHRESH3   ",
"LNA          ",
"RXBW         ",
"AFCBW        ",
"OOKPEAK      ",
"OOKAVG       ",
"OOKFIX       ",
"AFCFEI       ",
"AFCMSB       ",
"AFCLSB       ",
"FEIMSB       ",
"FEILSB       ",
"RSSICONFIG   ",
"RSSIVALUE    ",
"DIOMAPPING1  ",
"DIOMAPPING2  ",
"IRQFLAGS1    ",
"IRQFLAGS2    ",
"RSSITHRESH   ",
"RXTIMEOUT1   ",
"RXTIMEOUT2   ",
"PREAMBLEMSB  ",
"PREAMBLELSB  ",
"SYNCCONFIG   ",
"SYNCVALUE1   ",
"SYNCVALUE2   ",
"SYNCVALUE3   ",
"SYNCVALUE4   ",
"SYNCVALUE5   ",
"SYNCVALUE6   ",
"SYNCVALUE7   ",
"SYNCVALUE8   ",
"PACKETCONFIG1", 
"PAYLOADLENGTH", 
"NODEADRS     ",
"BROADCASTADRS", 
"AUTOMODES    ",
"FIFOTHRESH   ",
"PACKETCONFIG2", 
"AESKEY1      ",
"AESKEY2      ",
"AESKEY3      ",
"AESKEY4      ",
"AESKEY5      ",
"AESKEY6      ",
"AESKEY7      ",
"AESKEY8      ",
"AESKEY9      ",
"AESKEY10     ",
"AESKEY11     ",
"AESKEY12     ",
"AESKEY13     ",
"AESKEY14     ",
"AESKEY15     ",
"AESKEY16     ",
"TEMP1        ",
"TEMP2        ",
"TESTPA1      ", 
"TESTPA2      ", 
"TESTDAGC     ",
};

byte RegList [] ={
REG_OPMODE			    ,
REG_DATAMODUL	  	  ,
REG_BITRATEMSB		  ,
REG_BITRATELSB		  ,
REG_FDEVMSB			    ,
REG_FDEVLSB		  	  ,
REG_FRFMSB	  		  ,
REG_FRFMID			    ,
REG_FRFLSB		  	  ,
REG_OSC1		  	    ,
REG_AFCCTRL 		    ,
REG_LOWBAT			    ,
REG_LISTEN1			    ,
REG_LISTEN2			    ,
REG_LISTEN3			    ,
REG_VERSION			    ,
REG_PALEVEL			    ,
REG_PARAMP			    ,
REG_OCP				      ,
REG_AGCREF			    ,
REG_AGCTHRESH1		  ,
REG_AGCTHRESH2		  ,
REG_AGCTHRESH3		  ,
REG_LNA			  	    ,
REG_RXBW		    	  ,
REG_AFCBW		    	  ,
REG_OOKPEAK	  		  ,
REG_OOKAVG  			  ,
REG_OOKFIX	  		  ,
REG_AFCFEI		  	  ,
REG_AFCMSB			    ,
REG_AFCLSB			    ,
REG_FEIMSB			    ,
REG_FEILSB			    ,
REG_RSSICONFIG		  ,
REG_RSSIVALUE		    ,
REG_DIOMAPPING1		  ,
REG_DIOMAPPING2		  ,
REG_IRQFLAGS1		    ,
REG_IRQFLAGS2		    ,
REG_RSSITHRESH		  ,
REG_RXTIMEOUT1		  ,
REG_RXTIMEOUT2		  ,
//REG_PREAMBLEMSB		  ,
//REG_PREAMBLELSB		  ,
//REG_SYNCCONFIG		  ,
REG_SYNCVALUE1		  ,
REG_SYNCVALUE2		  ,
//REG_SYNCVALUE3		  ,
//REG_SYNCVALUE4		  ,
//REG_SYNCVALUE5		  ,
//REG_SYNCVALUE6		  ,
//REG_SYNCVALUE7		  ,
//REG_SYNCVALUE8		  ,
//REG_PACKETCONFIG1	  ,
//REG_PAYLOADLENGTH	  ,
//REG_NODEADRS		    ,
//REG_BROADCASTADRS	  ,
//REG_AUTOMODES		    ,
//REG_FIFOTHRESH		  ,
//REG_PACKETCONFIG2	  ,
//REG_AESKEY1			    ,
//REG_AESKEY2 			  ,
//REG_AESKEY3	  		  ,
//REG_AESKEY4		  	  ,
//REG_AESKEY5			    ,
//REG_AESKEY6 			  ,
//REG_AESKEY7	  		  ,
//REG_AESKEY8		  	  ,
//REG_AESKEY9			    ,
//REG_AESKEY10  		  ,
//REG_AESKEY11	  	  ,
//REG_AESKEY12		    ,
//REG_AESKEY13  		  ,
//REG_AESKEY14	  	  ,
//REG_AESKEY15		    ,
//REG_AESKEY16		    ,
REG_TEMP1			      ,
REG_TEMP2			      ,
//REG_TESTPA1         ,
//REG_TESTPA2         ,
//REG_TESTDAGC        ,
0

};
void printDetail ( byte regAddr , byte regVal )
{
  int capVal;

  //... State Variables for intelligent decoding
  byte  modeFSK = 0;
  int bitRate = 0;
  int freqDev = 0;
  long freqCenter = 0;

    switch ( regAddr ) 
    {
        case 0x1 : {
            SerialPrint ( "Controls the automatic Sequencer ( see section 4.2 ) SequencerOff : " );
            if ( 0x80 & regVal ) {
                SerialPrint ( "1 -> Mode is forced by the user\n" );
            } else {
                SerialPrint ( "0 -> Operating mode as selected with Mode bits in RegOpMode is automatically reached with the Sequencer\n" );
            }
            
            SerialPrint( " Enables Listen mode, should be enabled whilst in Standby mode: ListenOn : " );
            if ( 0x40 & regVal ) {
                SerialPrint ( "1 -> On\n" );
            } else {
                SerialPrint ( "0 -> Off ( see section 4.3)\n" );
            }
            
            SerialPrint( " Aborts Listen mode when set together with ListenOn=0 See section 4.3.4 for details (Always reads 0.) " );
            if ( 0x20 & regVal ) {
                SerialPrint ( "ERROR - ListenAbort should NEVER return 1 this is a write only register\n" );
            }
            
            SerialPrint(" Transceiver's operating modes: Mode : ");
            capVal = (regVal >> 2) & 0x7;
            if ( capVal == 0 ) {
                SerialPrint ( "000 -> Sleep mode (SLEEP)\n" );
            } else if ( capVal = 1 ) {
                SerialPrint ( "001 -> Standby mode (STDBY)\n" );
            } else if ( capVal = 2 ) {
                SerialPrint ( "010 -> Frequency Synthesizer mode (FS)\n" );
            } else if ( capVal = 3 ) {
                SerialPrint ( "011 -> Transmitter mode (TX)\n" );
            } else if ( capVal = 4 ) {
                SerialPrint ( "100 -> Receiver Mode (RX)\n" );
            } else {
                Serial.print( capVal, BIN );
                SerialPrint ( " -> RESERVED\n" );
            }
            break;
        }
        
        case 0x2 : {
        
            SerialPrint("Data Processing mode: DataMode : ");
            capVal = (regVal >> 5) & 0x3;
            if ( capVal == 0 ) {
                SerialPrint ( "00 -> Packet mode\n" );
            } else if ( capVal == 1 ) {
                SerialPrint ( "01 -> reserved\n" );
            } else if ( capVal == 2 ) {
                SerialPrint ( "10 -> Continuous mode with bit synchronizer\n" );
            } else if ( capVal == 3 ) {
                SerialPrint ( "11 -> Continuous mode without bit synchronizer\n" );
            }
            
            SerialPrint(" Modulation scheme: Modulation Type : ");
            capVal = (regVal >> 3) & 0x3;
            if ( capVal == 0 ) {
                SerialPrint ( "00 -> FSK\n" );
                modeFSK = 1;
            } else if ( capVal == 1 ) {
                SerialPrint ( "01 -> OOK\n" );
            } else if ( capVal == 2 ) {
                SerialPrint ( "10 -> reserved\n" );
            } else if ( capVal == 3 ) {
                SerialPrint ( "11 -> reserved\n" );
            }
            
            SerialPrint(" Data shaping: ");
            if ( modeFSK ) {
                SerialPrint( "in FSK:\n" );
            } else {
                SerialPrint( "in OOK:\n" );
            }
            SerialPrint ("ModulationShaping : ");
            capVal = regVal & 0x3;
            if ( modeFSK ) {
                if ( capVal == 0 ) {
                    SerialPrint ( "00 -> no shaping\n" );
                } else if ( capVal == 1 ) {
                    SerialPrint ( "01 -> Gaussian filter, BT = 1.0\n" );
                } else if ( capVal == 2 ) {
                    SerialPrint ( "10 -> Gaussian filter, BT = 0.5\n" );
                } else if ( capVal == 3 ) {
                    SerialPrint ( "11 -> Gaussian filter, BT = 0.3\n" );
                }
            } else {
                if ( capVal == 0 ) {
                    SerialPrint ( "00 -> no shaping\n" );
                } else if ( capVal == 1 ) {
                    SerialPrint ( "01 -> filtering with f(cutoff) = BR\n" );
                } else if ( capVal == 2 ) {
                    SerialPrint ( "10 -> filtering with f(cutoff) = 2*BR\n" );
                } else if ( capVal == 3 ) {
                    SerialPrint ( "ERROR - 11 is reserved\n" );
                }
            }
            
            break;
        }
        
        case 0x3 : {
            bitRate = (regVal << 8);
            break;
        }
        
        case 0x4 : {
            bitRate |= regVal;
            SerialPrint ( "Bit Rate (Chip Rate when Manchester encoding is enabled) BitRate : ");
            unsigned long val = 32UL * 1000UL * 1000UL / bitRate;
            Serial.println( val );
            break;
        }
        
        case 0x5 : {
            freqDev = ( (regVal & 0x3f) << 8 );
            break;
        }
        
        case 0x6 : {
            freqDev |= regVal;
            SerialPrint( "Frequency deviation Fdev : " );
            unsigned long val = 61UL * freqDev;
            Serial.println( val );
            break;
        }
        
        case 0x7 : {
            unsigned long tempVal = regVal;
            freqCenter = ( tempVal << 16 );
            break;
        }
       
        case 0x8 : {
            unsigned long tempVal = regVal;
            freqCenter = freqCenter | ( tempVal << 8 );
            break;
        }

        case 0x9 : {        
            freqCenter = freqCenter | regVal;
            SerialPrint ( "RF Carrier frequency FRF : " );
            unsigned long val = 61UL * freqCenter;
            Serial.println( val );
            break;
        }

        case 0xa : {
            SerialPrint ( "RC calibration control & status RcCalDone : " );
            if ( 0x40 & regVal ) {
                SerialPrint ( "1 -> RC calibration is over\n" );
            } else {
                SerialPrint ( "0 -> RC calibration is in progress\n" );
            }
        
            break;
        }

        case 0xb : {
            SerialPrint ( "Improved AFC routine for signals with modulation index lower than 2.  Refer to section 3.4.16 for details AfcLowBetaOn : " );
            if ( 0x20 & regVal ) {
                SerialPrint ( "1 -> Improved AFC routine\n" );
            } else {
                SerialPrint ( "0 -> Standard AFC routine\n" );
            }
            break;
        }
        
        case 0xc : {
            SerialPrint ( "Reserved \n" );
            break;
        }

        case 0xd : {
            byte val;
            SerialPrint ( "Resolution of Listen mode Idle time (calibrated RC osc): ListenResolIdle : " );
            val = regVal >> 6;
            if ( val == 0 ) {
                SerialPrint ( "00 -> reserved\n" );
            } else if ( val == 1 ) {
                SerialPrint ( "01 -> 64 us\n" );
            } else if ( val == 2 ) {
                SerialPrint ( "10 -> 4.1 ms\n" );
            } else if ( val == 3 ) {
                SerialPrint ( "11 -> 262 ms\n" );
            }
            
            SerialPrint ( " Resolution of Listen mode Rx time (calibrated RC osc): ListenResolRx : " );
            val = (regVal >> 4) & 0x3;
            if ( val == 0 ) {
                SerialPrint ( "00 -> reserved\n" );
            } else if ( val == 1 ) {
                SerialPrint ( "01 -> 64 us\n" );
            } else if ( val == 2 ) {
                SerialPrint ( "10 -> 4.1 ms\n" );
            } else if ( val == 3 ) {
                SerialPrint ( "11 -> 262 ms\n" );
            }

            SerialPrint ( " Criteria for packet acceptance in Listen mode: ListenCriteria : " );
            if ( 0x8 & regVal ) {
                SerialPrint ( "1 -> signal strength is above RssiThreshold and SyncAddress matched\n" );
            } else {
                SerialPrint ( "0 -> signal strength is above RssiThreshold\n" );
            }
            
            SerialPrint ( " Action taken after acceptance of a packet in Listen mode: ListenEnd : " );
            val = (regVal >> 1 ) & 0x3;
            if ( val == 0 ) {
                SerialPrint ( "00 -> chip stays in Rx mode. Listen mode stops and must be disabled (see section 4.3)\n" );
            } else if ( val == 1 ) {
                SerialPrint ( "01 -> chip stays in Rx mode until PayloadReady or Timeout interrupt occurs.  It then goes to the mode defined by Mode. Listen mode stops and must be disabled (see section 4.3)\n" );
            } else if ( val == 2 ) {
                SerialPrint ( "10 -> chip stays in Rx mode until PayloadReady or Timeout occurs.  Listen mode then resumes in Idle state.  FIFO content is lost at next Rx wakeup.\n" );
            } else if ( val == 3 ) {
                SerialPrint ( "11 -> Reserved\n" );
            }
            
            
            break;
        }
        
        default : {
        }
    }
}

//for debugging
void PrintReg(byte regAddr , byte regVal){
 int i;
    Serial.print("Reg : ");
    Serial.print(RegName[regAddr]);
    
    
    Serial.print(regAddr, HEX);
    Serial.print(" - ");
  if (regVal<16)
     Serial.print("0");
    Serial.print(regVal,HEX);
    Serial.print(" - ");
    //Serial.println(regVal,BIN);
    for (i=7;i>=0;i--){
      if ( regVal & (1<<i) ) 
      Serial.print("1");
     else
      Serial.print("0");
    }
    Serial.print(" - ");
    Serial.print(regVal,DEC);
    Serial.println(" ");
 
}

void PrintReg(byte regAddr )
	{
		byte regVal ;
    regVal = readReg (regAddr);
		PrintReg( regAddr ,  regVal);
	}

void readAllRegs()
{
  byte regVal;
 
  for (byte regAddr = 1; regAddr <= 0x4F; regAddr++)
 {
    
    regVal = readReg (regAddr);
  PrintReg( regAddr ,  regVal);
  printDetail( regAddr ,  regVal);
 }
}


void readListRegs(byte *regAddr)
{
  byte regVal;
 
 while ( *regAddr !=0 )
 {
    
    regVal = readReg (*regAddr);
		PrintReg( *regAddr ,  regVal);
//		printDetail( regAddr ,  regVal);
		regAddr++;
 }
}

