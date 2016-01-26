/**
 * HomeEasy Library
 *
 * Usage notes : 
 *   By default the library is hooked up to a fixed set of pins (for the benefit of the interrupts) and configured for a standard Arduino.
 *
 *   On a standard Arduino, you should connect the transmitter data to pin 13 and the receiver data to pin 8 
 *   - The transmission pin is configurable, by editing HomeEasyDefines.h - see http://www.arduino.cc/en/Hacking/PinMapping168 to select a port and pin
 *
 *   On an Arduino Mega, you should connect the transmitter data to pin 48 and the receiver data to pin 49
 *   - The receiving pin is configurable, by editing HomeEasyDefines.h - selecting HETIMER4 selects pin 49, and HETIMER5 selects pin 48
 *   - The transmission pin is configurable, by editing HomeEasyDefines.h - see http://arduino.cc/en/uploads/Main/arduino-mega2560-schematic.pdf to select a port and pin
 */
#include "HomeEasyDefines.h"
#include "HomeEasy.h"

  // variables used for receiving the messages
    word pulseWidth ;
    byte bitCount ;
    byte bit ;
    byte prevBit ;
    byte messageCount;

/**
 * Constructor
 */
HomeEasy::HomeEasy() {
 pulseWidth = 0;
 bitCount = 0;
 bit = 0;
 prevBit = 0;
 messageCount=0;

}

/*
CS12 CS11 CS10 Description
0 0 0 No clock source (Timer/Counter stopped).
0 0 1 clkI/O/1 (No prescaling)
0 1 0 clkI/O/8 (From prescaler)
0 1 1 clkI/O/64 (From prescaler)
1 0 0 clkI/O/256 (From prescaler)
1 0 1 clkI/O/1024 (From prescaler)
1 1 0 External clock source on T1 pin. Clock on falling edge.
1 1 1 External clock source on T1 pin. Clock on rising edge.
*/


void Timer1Init(word micros )
{
    // initialize Timer1
    cli();             // disable global interrupts
    TIFR1  = 0xFF;     //clear interrupt
    TCCR1B = 0;        //stoped timer
    TCNT1  = 0;
    TCCR1A = 0;        // set entire TCCR1A register to 0

    // turn on CTC mode:
    TCCR1B |= (1 << WGM12);

    // enable timer compare interrupt:
    //TIMSK1 |= (1 << OCIE1A);
    
    // set compare match register to desired timer count:
    OCR1A = micros*2;
         
    // Set CS11 bit 
    // set prescaler to 1/8.  HE_TCNT increments every 0.5 micro seconds
    //start timer
//    TCCR1B = (1 << CS11);

    TCCR1B |=  5 ;  // 1 0 1 clkI/O/1024 (From prescaler) = 64 ms


    // enable global interrupts:
    sei();
}

void Timer1Compare1Wait()
{
  //wait interrupt compare flag
  while(TIFR1==0);
  TIFR1  = 0xFF;     //clear interrupt
}

void DelayMicros(word micros)
{
  Timer1Init( micros ) ;
  Timer1Compare1Wait() ;
}

/**
 * Initialise the system.
 * 
 * Enables the receiving of messages.
 */
void HomeEasy::init() {
  // ensure the receiver pin is set for input
  HE_RXDDR &= ~_BV(HE_RXPIN);
  
  // disable PWM (default)
  HE_TCCRA = 0x00;
  
  // set prescaler to 1/8.  HE_TCNT increments every 0.5 micro seconds
  // falling edge used as trigger
  HE_TCCRB = _BV(CS21);
  
  HE_TCCRB =  5 ;  // 1 0 1 clkI/O/1024 (From prescaler) = 64 ms
  
  
  // enable input capture interrupt for HETIMER
  HE_TIMSK = _BV(HE_ICIE);
  pulseWidth = 0;
  bitCount = 0;
  bit = 0;
  prevBit = 0;
  messageCount=0;  
}

/**
 * Reconfigure the interrupts for sending a message.
 */
void HomeEasy::initSending() {
  // reset counter
  HE_TCNT = 0;

  // ensure the transmitter pin is set for output
  HE_TXDDR |= _BV(HETXPIN);

  // the value that the timer will count up to before firing the interrupt
  HE_OCRA = (pulseWidth * 2);

  // do not toggle OCxA on compare match, do it manually otherwise we get out of sync
  HE_TCCRA = 0;

  // CTC mode: top of HE_OCRA, immediate update of HE_OCRA, TOVx flag set on MAX
  HE_TCCRB |= _BV(HE_WGM2);

  // enable timer interrupt for HETIMER, disable input capture interrupt
  HE_TIMSK = _BV(HE_OCIEA);
}

/**
 * The input interrupt handler.
 *
 * This is where the message is received and decoded.
 */
ISR(HE_TIMER_CAPT_vect)
{
	// reset counter
	HE_TCNT = 0;
	
	// get value of input compare register, divide by two to get microseconds
	pulseWidth = (HE_ICR / 2);
	
	if(bit_is_clear(HE_TCCRB, HE_ICES))
	{	// falling edge was detected, HIGH pulse end
		
	}
	else
	{	// raising edge was detected, LOW pulse end
		
	}
	
	// toggle bit value to trigger on the other edge
	HE_TCCRB ^= _BV(HE_ICES);
}

/**
 *
 */
void HomeEasy::sendMessage(unsigned int s, unsigned int r, bool c)
{
	// disable all interrupts
	HE_TIMSK = 0;
	
	// reset variables
	messageCount = 0;
	bitCount = 0;
	bit = 0;
	prevBit = 0;
	pulseWidth = 16*1000/2 ; //16*64 micros = 1second
	
	
	// start the timer interrupt
	initSending();
}

/**
 * The timer interrupt handler.
 * 
 * This is where the message is transmitted.
 * 
 * The timer interrupt is used to wait for the required length of time.  Each call of this
 * function toggles the output and determines the length of the time until the function is
 * called again.
 * 
 * Once the message has been transmitted this class will switch back to receiving.
 */
#define LED  9


ISR(HE_TIMER_COMPA_vect)
{
		if(prevBit==0 && bitCount != 25)
		{
		//	HE_TXPORT |= _BV(HETXPIN);
                        prevBit=1;

		}
		else
		{
		//	HE_TXPORT &= ~_BV(HETXPIN);
                        prevBit=0;
		}
                digitalWrite( LED, prevBit);   // turn the LED 
		
		
                bitCount++;
		if(bitCount == 25)
		{	// message finished
			Serial.println();
			bitCount = 0;
			messageCount++;
			
			if(messageCount == TRANSMITTER_MESSAGE_COUNT)
			{	// go back to receiving
				
				messageCount = 0;
				
				HE_TCCRA = 0x00;
				HE_TCCRB = 0x02;
				HE_TIMSK = _BV(HE_ICIE);
                                digitalWrite( LED, 0);   // turn the LED 
				
				return;
			}
		}
	
	// set the next delay
        HE_TCNT = 0;
 	HE_OCRA = (pulseWidth * 2);
        Serial.print(prevBit);
}
