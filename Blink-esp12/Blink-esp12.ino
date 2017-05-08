/*
 ESP8266 Blink by Simon Peter
 Blink the blue LED on the ESP-01 module
 This example code is in the public domain
 
 The blue LED on the ESP-01 module is connected to GPIO1 
 (which is also the TXD pin; so we cannot use Serial.print() at the same time)
 
 Note that this sketch uses LED_BUILTIN to find the pin with the internal LED
*/
#define LED 16
byte i ;
void setup() {
  pinMode(LED, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  Serial.begin(74880);
 delay(100);
    Serial.println();
    Serial.println(F("ESP starting."));
i=2;
}

// the loop function runs over and over again forever
void loop() {
  pinMode(i, OUTPUT);     // Initialize the LED_BUILTIN pin as an output

  digitalWrite(i, LOW);   // Turn the LED on (Note that LOW is the voltage level
                                    // but actually the LED is on; this is because 
                                    // it is acive low on the ESP-01)
  delay(1000);                      // Wait for a second
  digitalWrite(i, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(2000);                      // Wait for two seconds (to demonstrate the active low LED)
  Serial.println(F(".."));
}
