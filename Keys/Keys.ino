
#include <SPI.h>

#include "MAX7219.h"


#define MAXPIN 10
//Led output
#define IOCARD_OUT_CMD      (0x10|0x80)
//display output
#define IOCARD_DISPLAY_CMD  (0x20|0x80)


/******************************************************************************
 * Private variable declaration.
 ******************************************************************************/
//cs/load  = pin 10
static MAX7219 max7219(10);


byte CmdIn;
byte KpinNo; 
byte Koutpin;

//Old state pin array
byte OldStatePins[MAXPIN];

void InitMax7219()
{
    // Initiliaze MAX7219.
    max7219.initialize();
    // Set scan limit.
    max7219.setScanLimit(MAX7219::ScanLimit_Digit0To7);
     // Set decode mode.
    max7219.setDecodeMode(MAX7219::DecodeMode_AllDigits);
    // Set intensity.
    max7219.setIntensity(MAX7219::Intensity_Level15);
    // Set shutdown mode.
    max7219.setShutdownMode(MAX7219::ShutdownMode_NormalOperation);
}

void InitInputPort()
{
  for (int KoutPin = 2; KoutPin < MAXPIN; KoutPin++)// Get all the pins ready for "Keys"  
  {
    if (KoutPin!=13)
    {
    pinMode(KoutPin, INPUT_PULLUP );
    digitalWrite(KoutPin, HIGH);  
    }
  }
}

void setup() 
{
  memset (OldStatePins,1,sizeof(OldStatePins) );
  CmdIn=0;
  InitInputPort();
  InitMax7219();
  
 Serial.begin(115200);  
Serial.println("run"); 
}

void SetLed(byte LedNumber, byte Value )
{
  if (Value)
    digitalWrite(LedNumber, HIGH); 
  else
    digitalWrite(LedNumber, LOW ); 
    
}
void SetDigit(byte DigitNumber, byte Value )
{
     // Write digit value.
    max7219.writeDigit((MAX7219::Digit_t)(DigitNumber & 0x7), (MAX7219::Character_t)(Value & 0xF) );
}

void ManageOutput(byte CmdIn , byte outputNumber)
{
    byte Cmd  = GetOutputCmd(&CmdIn);
    byte Value= GetOutputValue(&CmdIn);
    //led output
    if ( (Cmd==IOCARD_OUT_CMD))
    {
      //READ led output number
      //send led on/of
      SetLed( outputNumber, Value );
    }
    if ( (Cmd==IOCARD_DISPLAY_CMD))
    {
      //send display
      SetDigit(outputNumber, Value );
    }

}
void loop() {
  byte cmd;
  {KEYS();} //Check the "keys" section
  if (Serial.available()) {
    cmd = Serial.read();
    if ((cmd==IOCARD_OUT_CMD) || (cmd==IOCARD_DISPLAY_CMD) ) 
      CmdIn=cmd ;
    else 
      ManageOutput(CmdIn,cmd);
  }
  delay(10);//1ms

}

//SwNumber : 0..127
//SwValue 0..1
void SetSwitchValue(byte * Buffer , byte SwNumber , byte SwValue  )
{
  if (SwValue)
	  Buffer[0] = (byte)0x80 ;
  else
	  Buffer[0] = 0   ;
  Buffer[0] |= SwNumber ;

}


void KEYS() 
{
byte pinState , oldPinState ;
byte buf[1];

  for (byte KpinNo = 2; KpinNo < MAXPIN; KpinNo++){
    pinState    = digitalRead(KpinNo); 
    oldPinState = OldStatePins[KpinNo];

    if (pinState != oldPinState)
    {
      SetSwitchValue( buf , KpinNo , pinState  );
      Serial.write(buf,1);
      //Serial.print (buf[0],HEX ); Serial.print (' ' ); 
      OldStatePins[KpinNo] = pinState ;
    }
  }
}
//return command IOCARD_OUT_CMD or IOCARD_DISPLAY_CMD
byte GetOutputCmd ( byte * Buffer)
{
  return (Buffer[0] & 0xF0 );
}
//return command value 0..15 
byte GetOutputValue ( byte * Buffer)
{
  return ( Buffer[0] & 0xF ) ;
}
//return output display number 0..127
byte GetOutputNumber ( byte * Buffer)
{
  return ( Buffer[1] );
}









