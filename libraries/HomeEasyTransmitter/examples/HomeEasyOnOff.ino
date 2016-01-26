#include <HomeEasyTransmitter.h>

// This is the Home Easy controller
// This example will use a 433AM transmitter on
// pin 4 and will flash an LED on pin 13 when
// transmitting
HomeEasyTransmitter easy(4);

// This is just a pin which has a push button
// connected, to trigger a Home Easy device
const int buttonPin = 6;

int buttonState = 0;
  
void setup()
{
  pinMode(buttonPin, INPUT);
}

void loop()
{
  buttonState = digitalRead(buttonPin);
  
  if (buttonState == LOW) {
    easy.setSwitch(true,55,1);    // turn on device 0
    delay(3000);        // wait 3 sec
    easy.setSwitch(false,55,1);    // turn on device 0
  }
}
