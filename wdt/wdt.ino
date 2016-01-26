#include <avr/wdt.h>
#define PIN  9

void setup ()
{
pinMode(PIN, OUTPUT);       
  digitalWrite(PIN,HIGH);
  Serial.begin (38400);
  Serial.println ("Restarted.");
  wdt_enable (WDTO_8S);  // reset after one second, if no "pat the dog" received
 }  // end of setup

void loop ()
{
  wdt_reset ();  // give me another second to do stuff (pat the dog)
  if (Serial.available() ){
    Serial.println ("Reset ...");
    digitalWrite(PIN,LOW);
    delay(1000);
    digitalWrite(PIN,HIGH);
    
    while (true) ;   // oops, went into a loop
  }
}  // end of loop
