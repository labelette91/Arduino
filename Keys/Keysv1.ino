
/* 
    This code is in the public domain
    For use with "Link2fs_Multi"
    Jimspage.co.nz
    My thanks to the Guys that gave me snippets of code. 
    
    This sets the complete Arduino Mega card for "keys" input except pin 13.
    Attach a switch to any pin (except 13) and program that pin in "Multi"
    Everything to do with "Keys"  starts with a "K" in this code.
    
    To change this for a Uno,, do the following ,,,,
    Line 29 ,, change "70"  to "20"
    Line 117 ,, change "70"  to  "20"
    That will make all the pins on a Uno as "Keys" input pins. (Except pin 13)

*/

#define MAXPIN 10

byte CodeIn;// used on all serial reads
byte KpinNo; 
byte Koutpin;

byte OldStatePins[MAXPIN];

void setup() 
{
  memset (OldStatePins,1,sizeof(OldStatePins) );
  
  for (int KoutPin = 2; KoutPin < MAXPIN; KoutPin++)// Get all the pins ready for "Keys"  
  {
    if (KoutPin!=13)
    {
    pinMode(KoutPin, INPUT_PULLUP );
    digitalWrite(KoutPin, HIGH);  
    }
  }
 Serial.begin(115200);  
Serial.println("run"); 
}

void loop() {
  {KEYS();} //Check the "keys" section
  if (Serial.available()) {
    CodeIn = Serial.read();
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









