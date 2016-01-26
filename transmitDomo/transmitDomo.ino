#include <RFM69.h>
#include <SPI.h>
#include <HomeEasyTransmitter.h>
#include "Lighting2.h"

#define NODEID      1
#define NETWORKID   100
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define LED         9
#define SERIAL_BAUD 38400

#define PDATA 3 //pin for data input/output
#define PCLK  4 //pin for clk  input/output

RFM69 radio;

// This is the Home Easy controller
// This example will use a 433AM transmitter on
// pin 3 : data pin
// pin 4 : clk  pin
HomeEasyTransmitter easy(PDATA,PCLK,LED);


void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(10);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  char buff[50];
  sprintf(buff, "\nListening at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);
  pinMode(PDATA, INPUT);
  pinMode(LED, OUTPUT);
 
  detachInterrupt(1);
  easy.initPin();
  radio.setMode(RF69_MODE_TX);
  delay(1000);        // wait 3 sec

  easy.setSwitch(true,55,1);    // turn on device 0
  delay(3000);        // wait 3 sec
  easy.setSwitch(false,55,1);    // turn on device 0

}

#define HEXTODEC(AH) AH = AH-'0'; if (AH>9) AH = AH -( 7 );if (AH>15) AH = AH - 0x20 ;

void loop() {

  if (DomoticReceive())
  {
  	Blink(9,250);
    easy.setSwitch(Cmd.LIGHTING2.cmnd,Cmd.LIGHTING2.id2,Cmd.LIGHTING2.unitcode);    // turn on device 0
  }
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
//  delay(DELAY_MS);
}
