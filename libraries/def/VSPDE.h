//--------------------------------------------------------------------
// Arduino Console Stub
//--------------------------------------------------------------------

#if _MSC_VER 

#include <string>

#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "arduino.h"

#pragma warning(disable : 4996)

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

// Add CPU hardware definitions

#define _AVR_IO_H_
#define _SFR_IO8(io_addr) ((io_addr))
//#include "C:\Program Files\Arduino\hardware\tools\avr\avr\include\avr\iomxx0_1.h"


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
	void print(unsigned long value);
	void print(float val);

	void println();
	void println(const char*);
	void println(int,int);
	void print(int value);
	void print(byte value);

	void println(unsigned int,int);
	void println(unsigned long,int);
	void println(byte value);
	int available();
	char read();
    static void begin(int){ return ; };

	// VSPDE
	void _append(char c);
private:
	char buffer[1024];
	int buflen;
};

extern CSerial Serial1;
extern void cli();
extern void sei();

char* decoupe(char* ligne, char* mot, char* sep);

char* decoupe(char* ligne, char* mot, int MotLen, char* separator, char* concatenator, bool AllowDuplicateSeparator);

char* decoupecsv(char* ligne, char* mot, int MotLen, char* sep);

void Split ( char * ptligne , char * separateur , char * concatenator , bool AllowDuplicateSeparator , std::string list[] );

#endif
