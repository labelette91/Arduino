#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include <wiringPi.h>
    #include <stdint.h>
    #include <stdio.h>
		#include <sched.h>    
    typedef unsigned char boolean;
    typedef unsigned char byte;
#endif

extern "C" void delayMicrosecondsHard (unsigned int howLong);



#include "HomeEasyTransmitter.h"

void scheduler_realtime() {

struct sched_param p;
p.__sched_priority = sched_get_priority_max(SCHED_RR);
if( sched_setscheduler( 0, SCHED_RR, &p ) == -1 ) {
printf("Failed to switch to realtime scheduler.");
}
}

void scheduler_standard() {

struct sched_param p;
p.__sched_priority = 0;
if( sched_setscheduler( 0, SCHED_OTHER, &p ) == -1 ) {
printf("Failed to switch to normal scheduler.");
}
}




int main(int argc, char *argv[])
{
    int TXPIN = 0;


    if(wiringPiSetup() == -1)
        {
            return 0;
            printf("failed wiring pi\n");
        }	

  HomeEasyTransmitter *easy = new HomeEasyTransmitter(TXPIN,0);
  easy->initPin();
	
//	scheduler_realtime();    
    
  printf("esay init \n");

//	if ( piHiPri (99) !=0)
//		printf("error setting priority  \n");

   digitalWrite (0, HIGH) ; 
  delayMicrosecondsHard (275);
   digitalWrite (0,  LOW) ; 
  delay(10);
   
  
  easy->setSwitch(true,0x55,1);    // turn on device 0
    delay(3000);
  easy->setSwitch(false,0x55,1);    // turn on device 0


	printf("fin du programme");    // execution terminée.


//	scheduler_standard();
}

