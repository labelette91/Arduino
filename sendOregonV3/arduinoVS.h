#ifndef arduinoVS
#define arduinoVS

#ifdef _MSC_VER

#define byte int
#define HIGH 1
#define LOW  0
#define OUTPUT 1
void delayMicroseconds(int) ;
void digitalWrite(int,int);
byte bitRead(int,int);
float round(float);
int digitalRead(int);
void pinMode(int,int);

#define boolean bool

long millis();
int analogRead(int);
int strlen(const char*);
double sqrt(double);
int delay(int);
#define uint8_t int


#include "Print.h"
extern Print Serial;

#endif

#else
#include "arduino.h"
#endif
