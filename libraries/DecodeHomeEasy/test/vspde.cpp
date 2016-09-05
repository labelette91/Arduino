//--------------------------------------------------------------------
// Arduino Console Stub
//--------------------------------------------------------------------

#if _MSC_VER 

#include "VSPDE.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>



extern void setup();
extern void loop();

CSerial Serial;

//--------------------------------------------------------------------
// Timers
//--------------------------------------------------------------------
unsigned int millis()
{
//	return (clock() * 1000)  /* /  CLOCKS_PER_SEC */;
  return 1;
}

void delay(unsigned long delayms)
{
	unsigned long u = millis() + delayms;
	while (u > millis())
		;
}

//--------------------------------------------------------------------
// I/O
//--------------------------------------------------------------------

void pinMode(int,int)
{
}

void digitalWrite(int,int)
{
}

bool digitalRead(int)
{
	return 0;
}

//--------------------------------------------------------------------
// Serial
//--------------------------------------------------------------------

void CSerial::begin(long)
{
	buffer[0] = 0;
	buflen = 0;
}

void CSerial::print(const char *pString)
{
	printf("%s", pString);
}
void CSerial::print(const char pString)
{
	printf("%c", pString);
}

void CSerial::print(int value, int f)
{
  if (f==HEX)
	printf("%x", value);
  else
	printf("%d", value);

}

void CSerial::println()
{
	printf("\r\n");
}

void CSerial::println(char *pString)
{
	printf("%s\r\n", pString);
}

void CSerial::println(int value, int)
{
	printf("%d\r\n", value);
}

void CSerial::println(unsigned int value, int)
{
	printf("%u\r\n", value);
}

void CSerial::println(unsigned long value, int)
{
	printf("%lu\r\n", value);
}


int CSerial::available() 
{
	return buflen;
}

char CSerial::read() 
{ 
	char c = 0;
	if (buflen > 0)
	{
		c = buffer[0];
		memcpy(&buffer[0], &buffer[1], --buflen);
	}
	return c;
}
void CSerial::_append(char c)
{
	CSerial::buffer[buflen] = c;
	if (++buflen >= 1024)
	{
		buflen--;
	}
}

//--------------------------------------------------------------------
// Main
//--------------------------------------------------------------------

int main(int, char**)
{
	setup();
	for(;;)
	{
		if (_kbhit())
		{
			Serial._append((char)_getch());
		}
		loop();
	}
}

int micros(){return 1;};
 void cli(){};
 void sei(){};

#endif