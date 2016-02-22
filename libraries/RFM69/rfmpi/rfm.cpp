#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include <wiringPi.h>
		#include <wiringPiSPI.h>
    #include <stdint.h>
    #include <stdio.h>
		#include "Print.h"
    typedef unsigned char boolean;
    typedef unsigned char byte;
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

extern "C" void delayMicrosecondsHard (unsigned int howLong);

#define	SPI_CHAN		1
static int myFd ;

void spiSetup (int speed)
{
  if ((myFd = wiringPiSPISetup (SPI_CHAN, speed)) < 0)
  {
    printf ( "Can't open the SPI bus: \n" ) ;
   
  }
}

//for debugging
void PrintReg(byte regAddr , byte regVal){
	int i;
    Serial.print(regAddr, HEX);
    Serial.print(" - ");
		if (regVal<16)
    	Serial.print("0");
    Serial.print(regVal,HEX);
    Serial.print(" - ");
    //Serial.println(regVal,BIN);
    for (i=7;i>=0;i--){
      if ( regVal & (1<<i) ) 
    		Serial.print("1");
    	else
    		Serial.print("0");
    }
    Serial.print(" - ");
    Serial.print(regVal,DEC);
    Serial.println(" ");
	
}

byte readReg(byte addr)
{
	  unsigned char myData[10] ;
		myData[0]=addr ;
		myData[1]=0  ;

		if (wiringPiSPIDataRW (SPI_CHAN, myData, 2) == -1)
		{
		  printf ("SPI failure: %s\n", strerror (errno)) ;
		}
		return(myData[1]);

}

void readAllRegs()
{
  byte regVal;
	
  for (byte regAddr = 1; regAddr <= 0x4F; regAddr++)
	{
    
    regVal = readReg (regAddr);
		PrintReg( regAddr ,  regVal);
	}
}



int main(int argc, char *argv[])
{
    int TXPIN = 0;
		 unsigned char myData[10] ;

    if(wiringPiSetup() == -1)
        {
            return 0;
            printf("failed wiring pi\n");
        }	

  spiSetup (  1000000) ;
  printf("spi init \n");

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
/*   digitalWrite (0, HIGH) ; 
  delayMicrosecondsHard (275);
   digitalWrite (0,  LOW) ; 
  delay(10);
  */ 
  
readAllRegs();

	printf("fin du programme");    // execution terminée.
  close (myFd) ;


//	scheduler_standard();
}

