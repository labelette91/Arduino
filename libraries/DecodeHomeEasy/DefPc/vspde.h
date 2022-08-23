//--------------------------------------------------------------------
// Arduino Console Stub
//--------------------------------------------------------------------

#if _MSC_VER 

#pragma once

#define _CRT_SECURE_NO_WARNINGS


#include "deftype.h"

#pragma warning(disable : 4996)

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

// Add CPU hardware definitions

#define _AVR_IO_H_
#define _SFR_IO8(io_addr) ((io_addr))
//#include "C:\Program Files\Arduino\hardware\tools\avr\avr\include\avr\iomxx0_1.h"

#define boolean bool

// From "c:\program files\arduino\hardware\cores\arduino\print.h"

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
#define BYTE 0

// From "c:\program files\arduino\hardware\cores\arduino\wiring.h"

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1

#define PI 3.14159265
#define HALF_PI 1.57079
#define TWO_PI 6.283185
#define DEG_TO_RAD 0.01745329
#define RAD_TO_DEG 57.2957786

#define SERIAL  0x0
#define DISPLAY 0x1

#define LSBFIRST 0
#define MSBFIRST 1

#define CHANGE 1
#define FALLING 2
#define RISING 3

#define INTERNAL 3
#define DEFAULT 1
#define EXTERNAL 0

class CSerial
{
public:
	void begin(long);

	void print(const char*);
	void print(const char);
	void print(int,int);

	void println();
	void println(char*);
	void println(int,int);
	void println(unsigned int,int);
	void println(unsigned long,int);
	int available();
	char read();

	// VSPDE
	void _append(char c);
private:
	char buffer[1024];
	int buflen;
};

extern CSerial Serial;
extern "C" {
extern unsigned int millis();
extern void delay(unsigned long);
extern void pinMode(int,int);
extern void digitalWrite(int,int);
extern bool digitalRead(int);
extern int micros();
extern void cli();
extern void sei();
}
#endif
