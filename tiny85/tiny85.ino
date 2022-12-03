// the setup routine runs once when you press reset:
#define PIN 1
void setup() {                
  // initialize the digital pin as an output.
  pinMode(PIN, OUTPUT); //0 LED on Model B
//  pinMode(1, OUTPUT); //LED on Model A   
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//  digitalWrite(1, HIGH);
  delay(1000);               // wait for a second
  digitalWrite(PIN, LOW);    // turn the LED off by making the voltage LOW
//  digitalWrite(1, LOW); 
  delay(1000);               // wait for a second
}
