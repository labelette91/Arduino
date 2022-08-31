/*
 * IRremote: IRsendDemo - demonstrates sending IR codes with IRsend
 * An IR LED must be connected to Arduino PWM pin 3.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */


#include <IRremote.h>

IRsend irsend;
#define sendPin 13
byte n;

void setup()
{
 Serial.begin(9600);
 pinMode(sendPin, OUTPUT);
 
}

void loop() {
	 Serial.println("send");
	for (int i = 0; i < 2 ; i++) {
	// SAMSUNG: E0E040BF  power E0E048B7 chaine
		 irsend.sendSAMSUNG(0xE0E048B7 , 32);
		delay(40);
	}
	n++ ;
	if ((n%2)==0)
		digitalWrite(sendPin, HIGH);
		else
		digitalWrite(sendPin, LOW);
		
	delay(5000); //5 second delay between each signal burst
}
