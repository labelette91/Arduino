/*
 * Example program using the HomeEasy class.
 * Extends the receiver program by including a (poorly implemented) transmission
 * routine that accepts input from the Serial Port and transmits simple protocol
 * messages.
 */
#include <Arduino.h>
#include "HomeEasy.h"
#define LED  9

HomeEasy homeEasy;

void setup(void);
void loop(void);
byte i = 0 ;
/**
 * Set up the serial interface and the HomeEasy class.
 */
void setup(void) {
  Serial.begin(38400);
  pinMode(LED, OUTPUT);     

  Serial.print("run");
//  homeEasy.init();
//  homeEasy.sendMessage(1, 2,3);

}

/**
 * No processing is required in the loop for receiving, so constantly call sendSimpleMessage().
 */
void loop(void) {
  // Send message
  digitalWrite( LED, 0);   // turn the LED 
  DelayMicros( 16*1000/2 );  //16*64 micros = 1second
  digitalWrite( LED, 1);   // turn the LED 
  DelayMicros( 16*1000/2 );  //16*64 micros = 1second
  Serial.println(millis());
  Serial.println(TIMSK0);
  i++;
  if (i==10)
    TIMSK0=0;
  if (i==11)
    TIMSK0=1;
}


