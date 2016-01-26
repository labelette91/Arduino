#ifdef _MSC_VER
#include "..\portage.h"
#endif

#include "..\HomeEasyTransmitter.h"

// This is the Home Easy controller
// This example will use a 433AM transmitter on
// pin 4 and will flash an LED on pin 13 when
// transmitting
HomeEasyTransmitter easy(4,0,0);

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
    easy.setSwitch(true,55,1);    // turn on device 0

}



void main(){
setup();
loop();


}


int nbW = 0;
int nbD = 0;

void pinMode(uint8_t, uint8_t){};
void digitalWrite(uint8_t p , uint8_t v )
{printf("nb:%3d Pin:%d = %d ",nbW++,p,v);};
int digitalRead(uint8_t){return 0;};
int analogRead(uint8_t){return 0;};
void analogReference(uint8_t mode){};
void analogWrite(uint8_t, int){};

unsigned long millis(void){return 0;};
unsigned long micros(void){return 0;};
void delay(unsigned long d){printf("Delay:%d \n",d);};
void delayMicroseconds(unsigned int us){printf("Delay %3d micros:%d \n",nbD++,us);};
unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout){return 0;};
void cli(){};
void sei(){};
