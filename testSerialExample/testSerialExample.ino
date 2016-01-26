/*
  Software serial multple serial test
 
 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.
 
 The circuit: 
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 11 (connect to RX of other device)
 
 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts, 
 so only the following can be used for RX: 
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69
 
 Not all pins on the Leonardo support change interrupts, 
 so only the following can be used for RX: 
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
 
 created back in the mists of time
 modified 25 May 2012
 by Tom Igoe
 based on Mikal Hart's example
 
 This example code is in the public domain.
 
 */
#include <SoftwareSerial.h>

SoftwareSerial mySerial(0, 1); // RX, TX

uint8_t buf[16];

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 9;

void setup()  
{
  // Open serial communications and wait for port to open:
  
 
  // set the data rate for the SoftwareSerial port
  mySerial.begin(38400);
  
   // initialize the digital pin as an output.
  pinMode(led, OUTPUT); 
  
  buf[7]=0;

/* buf[0]='A';
 buf[1]='B';
 buf[2]=10;
 buf[3]=13;*/

TypFnAToB("0850052E480000E469",buf,9);  
//TypFnAToB("414243440A0D",buf,6);  
 
 
}


bool TypFnAToB (char * st, uint8_t bin[], uint8_t  trame_len)
{
  uint8_t  h,l ;
  uint8_t i;
  uint8_t index ;
                   /* -------------- */
  index=0;
  for (i=0; i<trame_len; i++)
  {
    h = (st[index] - '0');
    if (h>9)
       h = (h-7);
    if (h>15)
      h = (h-32);
    index++;
    l = (st[index] - '0');
    if (l>9)
      l = (l-7);
    if (l>15)
      l = (l-32);
  index++;
    bin[i] = ((h << 4) + l) ;
    
  }
  return true;
};

  int analogValue=0;
void loop() // run over and over
{


digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)

mySerial.write(buf,9);
buf[7]++;

delay(1000);

//    mySerial.println("Hello, world?");
//  mySerial.println(analogValue);       // print as an ASCII-encoded decimal
//  mySerial.println(analogValue, DEC);  // print as an ASCII-encoded decimal
//  mySerial.println(analogValue, HEX);  // print as an ASCII-encoded hexadecimal
//  mySerial.println(analogValue, OCT);  // print as an ASCII-encoded octal
//  mySerial.println(analogValue, BIN);  // print as an ASCII-encoded binary    
//  mySerial.write(buf,4);

  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(15000);               // wait for a second
  analogValue++;
}

