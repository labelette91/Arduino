// ook_rpi.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


/* ===================================================
C code : test.cpp
* ===================================================
*/

#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

std::string DeviceR = "/dev/gpiofreq";

#include "ookdecoder.h"
#include "oregon.h"
#include "print.h"

#ifndef DOMOTIC
#include  "reportserialascii.h"
#endif

OregonDecoderV2 orscV2;

byte data[4];

#define MAXS 4096
int pulse[MAXS];
int 	NbPulse;

void PulseLed()
{

}

void reportSerial(const char* s, class DecodeOOK& decoder)
{
#ifndef DOMOTIC
	reportSerialAscii("OSV2", decoder.getData(), decoder.pos);
#else            
	reportDomotic(decoder.getData(), decoder.pos);
#endif      
}


int ook_rpi_read_drv( int gpio)
{
	FILE* fp;
	std::string Device;

	Device = DeviceR + "17";
	printf("opening %s\n", Device.c_str() );

	fp = fopen(Device.c_str(), "r");
	if (fp == NULL) {
		printf("[ERROR] %s device not found - kernel driver must be started !!\n", Device.c_str());
		exit(1);
	}

	while (1) {
		int count = fread(pulse, 4, 2048, fp);


		if (count > 0)
		{
			for (int i = 0; i < count; i++)
			{
				word p = pulse[i];
				if (orscV2.nextPulse(p))
				{
					// -1 : on retire le byte de postambule
					if (checksum(orscV2.getData(), orscV2.pos - 1))
					{
						if ((data[3] != orscV2.data[3]) || (data[0] != orscV2.data[0]) || (data[1] != orscV2.data[1]) || (data[2] != orscV2.data[2]))
						{
							PulseLed();
							reportSerial("OSV2", orscV2);
							data[0] = orscV2.data[0];
							data[1] = orscV2.data[1];
							data[2] = orscV2.data[2];
							data[3] = orscV2.data[3];

					}
				}
					else
					{
#ifndef DOMOTIC
						Serial.println("Bad checksum");
						reportSerial("OSV2", orscV2);
#endif     
					}
					orscV2.resetDecoder();


			}
		}
		}
		usleep(10000l);
	}
	fclose(fp);
	return 0;
}




int main()
{
	ook_rpi_read_drv(17);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file


#ifdef WIN32

int usleep(int)
{
	return 0;

}
int millis()
{
	return 0;
}
#endif