#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include <wiringPi.h>
		#include <wiringPiSPI.h>
    #include <stdint.h>
    #include <stdio.h>
		#include "Print.h"
		#include <sched.h>    
    typedef unsigned char boolean;
    typedef unsigned char byte;
#endif

#define SS 0
#define RF69_IRQ_PIN 0 
#define RF69_IRQ_NUM 0 

#include "RFM69.h"
#include "RFM69registers.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "SPI.h"

extern "C" void delayMicrosecondsHard (unsigned int howLong);

Print Serial ;

#define	SPI_CHAN		1
static int myFd ;
#define SerialPrint(MESSAGE) printf(MESSAGE) 


RFM69 * radio;


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

void spiSetup (int channel , int speed)
{
  if ((myFd = wiringPiSPISetup (SPI_CHAN, speed)) < 0)
  {
    printf ( "Can't open the SPI bus: \n" ) ;
   
  }
}


byte readReg(byte addr)
{
	  unsigned char myData[10] ;
		myData[0]=addr ;
		myData[1]=0  ;

		if (wiringPiSPIDataRW (SPI_CHAN, myData, 2) == -1)
		{
		  printf ("SPI Read failure: %s\n", strerror (errno)) ;
		}
		return(myData[1]);

}

void writeReg(byte addr, byte value)
{
	  unsigned char myData[10] ;
		myData[0]=addr  | 0x80;
		myData[1]=value  ;

		if (wiringPiSPIDataRW (SPI_CHAN, myData, 2) == -1)
		{
		  printf ("SPI write failure: %s\n", strerror (errno)) ;
		}
}


#include "rfmPrint.cpp"

const byte CONFIG[][2] =
  {
    /* 0x01 */ { REG_OPMODE, RF_OPMODE_SEQUENCER_ON | RF_OPMODE_LISTEN_OFF | RF_OPMODE_STANDBY },
    /* 0x02 */ { REG_DATAMODUL, RF_DATAMODUL_DATAMODE_CONTINUOUSNOBSYNC | RF_DATAMODUL_MODULATIONTYPE_OOK | RF_DATAMODUL_MODULATIONSHAPING_00 }, //no shaping
};

int TXPIN = 5 ;

SPIClass SPI;

int main(int argc, char *argv[])
{
		byte vreg;
		 unsigned char myData[10] ;

    if(wiringPiSetup() == -1)
        {
            return 0;
            printf("failed wiring pi\n");
        }	

//  spiSetup ( 1, 500000) ;
  if (SPI.Setup (SPI_CHAN, 500000)) 
  {
    printf ( "Can't open the SPI bus: \n" ) ;
   
  }


  printf("spi init \n");
/*
  for (int i=1 ; i<10;i++)
  {
		myData[0]=i ;
		myData[1]=0  ;

		if (wiringPiSPIDataRW (SPI_CHAN, myData, 2) == -1)
		{
		  printf ("SPI failure: %s\n", strerror (errno)) ;
		}

		for (int i=0;i<2;i++)
			printf("%02X ", myData[i] );
		printf("\n");

	}
	*/
/*   digitalWrite (0, HIGH) ; 
  delayMicrosecondsHard (275);
   digitalWrite (0,  LOW) ; 
  delay(10);
  */ 
  
//readAllRegs();
readListRegs(RegList);

/* byte i=0;
 writeReg(CONFIG[i][0], CONFIG[i][1]);
 PrintReg ( CONFIG[i][0]  ) ;
i=1;
 writeReg(CONFIG[i][0], CONFIG[i][1]);
 PrintReg ( CONFIG[i][0]  ) ;

	do {
	writeReg(REG_SYNCVALUE1, 0xaa); 
	vreg = readReg(REG_SYNCVALUE1) ;
  PrintReg ( REG_SYNCVALUE1 , vreg ) ;

	}while (vreg != 0xaa);
*/	
	printf("Radio\n"); 
  radio = new RFM69 (0,0);
	printf("init\n"); 
  radio->initialize(RF69_433MHZ,1,100);
	readListRegs(RegList);

  radio->setMode(RF69_MODE_RX);
  PrintReg ( REG_OPMODE  ) ;

  HomeEasyTransmitter *easy = new HomeEasyTransmitter(TXPIN,0);
	
  printf("esay init \n");

	radio->setMode(RF69_MODE_TX);
  PrintReg ( REG_OPMODE  ) ;
	//attente une secone max pour emetre si emission en cours -80--> -70

	radio->WaitCanSend(-70);

	easy->initPin();

	printf("Easy On\n");  
  easy->setSwitch(true,0x55,1);    // turn on device 0
    delay(1000);
	printf("Easy Off\n");  
  easy->setSwitch(false,0x55,1);    // turn on device 0

//	radio->setMode(RF69_MODE_SLEEP);
//  PrintReg ( REG_OPMODE  ) ;

  radio->setMode(RF69_MODE_RX);
  PrintReg ( REG_OPMODE  ) ;

	easy->deactivatePin();

	printf("fin du programme");    // execution terminée.
//  close (myFd) ;
	SPI.end();

//	scheduler_standard();
}

