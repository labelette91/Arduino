/*
  Button
 
 Turns on and off a light emitting diode(LED) connected to digital  
 pin 13, when pressing a pushbutton attached to pin 2. 
 
 
 The circuit:
 * LED attached from pin 13 to ground 
 * pushbutton attached to pin 2 from +5V
 * 10K resistor attached to pin 2 from ground
 
 * Note: on most Arduinos there is already an LED on the board
 attached to pin 13.
 
 
 created 2005
 by DojoDave <http://www.0j0.org>
 modified 30 Aug 2011
 by Tom Igoe
 
 This example code is in the public domain.
 
 http://www.arduino.cc/en/Tutorial/Button
 */

// constants won't change. They're used here to 
// set pin numbers:
const int ledPin =  13;      // the number of the LED pin

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

//pin number for AIN1
#define AIN1 7

word pulse ;

ISR(ANALOG_COMP_vect) {
    static word last;
    // determine the pulse length in microseconds, for either polarity
    pulse = micros() - last;
    last += pulse;
    
    Serial.print (".");
}


void setup() {
  Serial.begin(38400);
  
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);      
  
  
//input sur AIN1
// AIN0 = bandgap ref

//ACME = 0 Analog Comparateur Multiplexer enable
    bitClear(ADCSRB,ACME);
//aden = 1 
    bitSet(ADCSRA,ADEN);
    
    // use analog comparator to switch at 1.1V bandgap transition
    bitSet(ACSR , ACBG );
    bitSet(ACSR , ACI  );
    bitSet(ACSR , ACIE );

//interrupt mode toggle
    bitClear(ACSR , ACIS1 );
    bitClear(ACSR , ACIS0 );


    pinMode(AIN1, INPUT_PULLUP);  // use the AIO pin

 //enable pull up 
 //PORTD : pullup  1 = active 0: tri state
 // PORTD |= 0x08;
 //DDRD : direction register : 1 = output 0 : input
    Serial.print ("ACSR:");
    Serial.println(ACSR,BIN);
    Serial.print ("ADCSRA:");
    Serial.println(ADCSRA,BIN);
    Serial.print ("ADCSRB:");
    Serial.println(ADCSRB,BIN);
    Serial.print ("DIDR1:");
    Serial.println(DIDR1,BIN);

    Serial.print ("DDRD:");
    Serial.println(DDRD,BIN);
    Serial.print ("PORTD:");
    Serial.println(PORTD,BIN);  
  
}

void loop(){
  // read the state of the pushbutton value:
  buttonState = digitalRead(AIN1);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {     
    // turn LED on:    
    digitalWrite(ledPin, HIGH);  
  } 
  else {
    // turn LED off:
    digitalWrite(ledPin, LOW); 
  }
}
