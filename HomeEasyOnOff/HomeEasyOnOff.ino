#include <arduino.h>
#include <HomeEasyTransmitter.h>
#include <RFM69.h>
#include <RFM69registers.h>
#include <SPI.h>

// This is the Home Easy controller
// This example will use a 433AM transmitter on
// pin 3 : data pin
// pin 4 : clk  pin
#define ledPin  9
#define PDATA 3 //pin for data input/output
#define PCLK  4 //pin for clk  input/output

HomeEasyTransmitter easy(3,4,9);

RFM69 radio;

typedef union tRBUF {

	
struct  T_LIGHTING2{
		byte	packetlength;   /* == 11 : longueur a partir de packettype */
		byte	packettype;     /* 17    */
		byte	subtype;
		byte	seqnbr;
		byte	id1;            /* id emetteur 0..3  */
		byte	id2;            /* id emetteur 0..FF */
		byte	id3;            /* id emetteur 0..FF */
		byte	id4;            /* id emetteur 0..FF */
		byte	unitcode;       /* unit 0.0xF        */
		byte	cmnd;           /* 0 : off 1:On      */
		byte	level;          /* dim level 0..15   */
		byte	rssi ;
		byte	data[2] ;
		
	} LIGHTING2;
};

typedef union tPAR {
byte Byte[4] ;
unsigned long Long;
};

  
tRBUF  Cmd ;

unsigned long getLightingId (){
tPAR v  ;
v.Byte[0]=Cmd.LIGHTING2.id4 ;
v.Byte[1]=Cmd.LIGHTING2.id3 ;
v.Byte[2]=Cmd.LIGHTING2.id2 ;
v.Byte[3]=Cmd.LIGHTING2.id1 ;
return v.Long;
}
  
void setup()
{
    Serial.begin(38400);
       // initialize the LED pin as an output:
    pinMode(9, OUTPUT);       
    radio.initialize(RF69_433MHZ,1,100);
    radio.writeReg(REG_LNA, RF_LNA_ZIN_50);
	    easy.initPin();
	    radio.setMode(RF69_MODE_TX);

Cmd.LIGHTING2.id1=1;
Cmd.LIGHTING2.id2=2;
Cmd.LIGHTING2.id3=3;
Cmd.LIGHTING2.id4=4;

unsigned long r = getLightingId ();

Serial.println(r,HEX);
}

void loop()
{
  
  if (Serial.available() > 0)
  {
    char input = Serial.read();
    if (input=='0')
    {
      easy.setSwitch(false,0x55,1);    // turn on device 0
    }
    else
    if (input=='1')
    {
      easy.setSwitch(true,0x55,1);    // turn on device 0
    }
    else
    if (input=='2')
    {
      easy.setSwitch(false,0xAA,2);    // turn on device 0
    }
    else
    if (input=='3')
    {
      easy.setSwitch(false,0xAA,2);    // turn on device 0
      easy.setSwitch(true,0xAA,2);    // turn on device 0
    }
    else
    if (input=='4')
    {
      easy.setSwitch(true,0xAA,2);    // turn on device 0
    }

   Serial.print("sent:");
   Serial.println(input);
 
 
  }
}
