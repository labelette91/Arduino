/*
 * HE300 Automatic Protocol Transmitter 'Simulator'
 * David Edmundson 21/11/2009
 *
 * This class allows use of the homeeasy automatic protocol to be used in any Arduino program
 *
 * Based heavily on demo code by
 * Martyn Henderson 02/10/2009  http://martgadget.blogspot.com
 *
 * The code has been moved into a class, for easy re-usage and has been optimised 
 * in regards to calculating and storing the transmitter address.
 *
 * You dont need to learn the Arduino into the socket, because it can 
 * pretend to be a transmitter you already have.
 *
 * Use the Automatic protocol reciever code above to find out
 * your transmitter address, and reciepient ID. 
 *
 * Original code from Martyn
 * *Only* tested with one HE300 transmitter (HE Address 272946) with its
 * slider switch set to 1 which appears to be 0 in fact.
 * Seems to work with a switch socket (HE302S) and can turn it on and off 
 * without the ardunio being learned into the socket.
 *
 * Edited code works with a HE305 transmitter, connecting to a HEXXX light fitting. The reciepient code for these 
 * appears to be 0b1010. I based my timings based on actual timings received from /my/ transmitter.
 * These seem to match the numbers used by Barnaby and Peter but not from Martyn. Adjust if needed.
 *
 * Cheers to Barnaby and Peter, because without the code above
 * that shows the receiver output this wouldnt have been possible!
 *
 * If you make something cool using HomeEasy and Arduino, please 
 * post back here with a link to your code so we can all enjoy.
 *
 */
 
/* Advanced Protocol
The Advanced protocol is less well documented than the simple protocol, and also less likely to work between brands.

Device Addressing Edit
Unlike the simple protocol, each device is assigned a larger identification code. Bits 0 -25 are unique to every transmitter device. 
Bit 26 is the Group Flag, and Bit 27 is the ON / OFF Flag, 0 = Off, 1 = On. This BIt is also used  when dimming. 
Bits 28 - 31, are the Group Code (up to 16 units can be in the same group) and Bits 32 - 35 are the dim level (16 Levels). 
The dim level bits are not sent when  basic ON / OFF  commands are sent, 
an ON command to a dimmable module will restore it to the level it was when last switched off. 
2 consecutive ON's will start a dimming module cycling up and down.

To send a dimming level a special modified bit is placed at bit 27 (See Specification)c

Encoding Edit
Manchester coding is used:

Data 0 = Manchester 01          Data 1 = Manchester 10

A Manchester 0 is a High for 275uS and Low for 275uS

A Manchester 1 is a High for 275uS and Low for 1225uS

So.......

Data 0 = High 275uS, Low 275uS, High 275uS, Low 1225uS

Data 1 = High 275uS, Low 1225uS, High 275uS, Low 275uS

A preamble is sent before each command which is High 275uS, Low 2675uS

When sending a dim level a special bit is placed in bit 27

Dim bit 27 = High 275uS, Low 275uS, High 275uS, Low 275uS. This seems a bit odd, and goes agianst the manchester coding specification !

Each packet is sent 4 of 5 times with a 10mS space in between each.
 */
 
#ifdef _MSC_VER
#include "portage.h"
#else
#include <stdio.h>
#include <Arduino.h>
#include <avr/pgmspace.h>
#include <timer.h>
#endif
#include "HomeEasyTransmitter.h"

//temps en �s
#define LATCH1_HIGH  270
#define LATCH1_LOW   9900
#define LATCH2_HIGH  270
#define LATCH2_LOW   2675


#define BIT_PULSE_HIGH  (275-20)
#define BIT_0_PULSE_LOW (275-20)
#define BIT_1_PULSE_LOW (1225-30)

//#define DelayMicroseconds(VALUE) delayMicroseconds(VALUE);Serial.println(VALUE); 
#define DelayMicroseconds(VALUE) delayMicroseconds(VALUE);
//#define DelayMicroseconds(VALUE) DelayMicros(VALUE);



  void rfm69_wait_t_data(void)
{
  /* t_data = 250ns = 4 insn at 16MHz */
#ifndef _MSC_VER
  __asm__ __volatile__ ("nop");
  __asm__ __volatile__ ("nop");
  __asm__ __volatile__ ("nop");
  __asm__ __volatile__ ("nop");
#endif
}

void HomeEasyTransmitter::rfm69_set_data(byte state)
{
  /* data is sampled on the rising edge of dclk/dio1 */

  digitalWrite(clkPin, LOW);
//  Serial.print(state); Serial.print(" ");
  digitalWrite(txPin, state);
  if (ledPin) digitalWrite(ledPin, state);
  rfm69_wait_t_data();
  digitalWrite(clkPin, HIGH);
  /* assume at least t_data from here */
}


HomeEasyTransmitter::HomeEasyTransmitter(short dataPin, short pclkPin , byte pLed )
{
  txPin  = dataPin;
  clkPin = pclkPin ;
  ledPin = pLed ;
}


//sends either an on/off signal to the main switch
//always seems to work best if we send it twice..

void HomeEasyTransmitter::setSwitch(bool on, unsigned long transmitterId, short recipient, byte level)
{

/*  transmit(on, transmitterId, recipient,level);
  delay(10);
  transmit(on, transmitterId, recipient,level);
  delay(10);
  transmit(on, transmitterId, recipient,level);
  delay(10);
  transmit(on, transmitterId, recipient,level);
  delay(10);
  transmit(on, transmitterId, recipient,level);
  delay(10);
*/
for (byte i=0;i<5;i++)
  transmit(on, transmitterId, recipient,level);

}

void HomeEasyTransmitter::setSwitch(bool on, unsigned long transmitterId, short recipient)
{
for (byte i=0;i<4;i++)
  transmit(on, transmitterId, recipient, 16);
}


//sends either an on/off signal to the main switch
void HomeEasyTransmitter::transmit(bool blnOn,unsigned long transmitterId, short recipient, byte level )
{
  int i;

  cli();

  pinMode(txPin, OUTPUT);      // transmitter pin.
  pinMode(clkPin, OUTPUT);      // transmitter pin.


  // Do the latch sequence.. 
/*  rfm69_set_data( HIGH);
  DelayMicroseconds(LATCH1_HIGH);     
  rfm69_set_data( LOW);
  DelayMicroseconds(LATCH1_LOW);     // low for 9900 for latch 1
*/
  
  rfm69_set_data( HIGH);
  DelayMicroseconds(LATCH2_HIGH);     
    rfm69_set_data( LOW);
  DelayMicroseconds(LATCH2_LOW);     // low for 2675 for latch 1


  // Send HE Device Address..
  // This is a 26 bit code. 
  // Start at MSB and iterate through to the lowest
  for(i=25; i>=0; i--)
  {
    //The typecasting seems a bit overkill, but 26 bits needs a long and the arduino compiler seems to love trying to 
    //convert everything to an standard int.
    //creates bitmask of only relevant bit. Check and send a 1 or 0 as applicable.
    bool bitToSend = (unsigned long)(transmitterId & ((unsigned long)1 << i)) != 0;
    sendPair(bitToSend);

  }
	
  // Send 26th bit - group 1/0
  sendPair(false);

	//if dim level / special bit 27
	if (level<16)
		{
			sendBit(0);
			sendBit(0);
		}	
	else
		// Send 27th bit - on/off 1/0
  	sendPair(blnOn);

  // last 4 bits - recipient   -- button 1 on the HE300 set to 
  // slider position I in this example:
  for(i=3; i>=0; i--)
  {
    bool bitToSend = ( byte)(recipient & ((byte )1 << i)) != 0;
    sendPair(bitToSend);
  }

	//if dim level 
	if (level<16)
	{
		// slider position :
		for(i=3; i>=0; i--)
		{
		  bool bitToSend = ( byte)(level & ((byte )1 << i)) != 0;
		  sendPair(bitToSend);
		}
	
  }

/*    sendPair(true);
    sendPair(false);
    sendPair(true);
    sendPair(false);*/

  rfm69_set_data( HIGH);   // high again (shut up)
  DelayMicroseconds(LATCH1_HIGH);      // wait a moment
  rfm69_set_data( LOW);    // low again for 9900 - pause .
  DelayMicroseconds(LATCH1_LOW);      // wait a moment
  sei();

}

void HomeEasyTransmitter::transmit(bool blnOn,unsigned long transmitterId, short recipient  )
{
 transmit( blnOn, transmitterId, recipient, 16 );
	
}

void HomeEasyTransmitter::sendBit(bool b)
{
  if (b)
  {
    rfm69_set_data( HIGH);
    DelayMicroseconds(BIT_PULSE_HIGH);
    rfm69_set_data( LOW);
    DelayMicroseconds(BIT_1_PULSE_LOW);
  }
  else
  {
    rfm69_set_data( HIGH);
    DelayMicroseconds(BIT_PULSE_HIGH);
    rfm69_set_data( LOW);
    DelayMicroseconds(BIT_0_PULSE_LOW);
  }
}

void HomeEasyTransmitter::sendPair(bool b)
{
  // Send the Manchester Encoded data 01 or 10, never 11 or 00
  if(b)
  {
    sendBit(true);
    sendBit(false);
  }
  else
  {
    sendBit(false);
    sendBit(true);
  }
}


 void HomeEasyTransmitter::initPin()
 	{
    pinMode(txPin, OUTPUT);      // transmitter pin.
    pinMode(clkPin, OUTPUT);      // transmitter pin.
    digitalWrite(clkPin, LOW);
    digitalWrite(txPin, LOW);    //no tx 

 	}