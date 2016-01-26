#ifndef arduinoVS
#define arduinoVS


#if defined(_MSC_VER) || defined(__BCPLUSPLUS__)

#define byte int
#define HIGH 1
#define LOW  0
#define OUTPUT 1
#define boolean int

void delayMicroseconds(int) ;
void digitalWrite(int,int);
byte bitRead(int,int);
float round(float);
int digitalRead(int);
void pinMode(int,int);

long millis();
int analogRead(int);
int strlen(const char*);
double sqrt(double);
int delay(int);
#define uint8_t int


#include "Print.h"
extern Print Serial;

#else
#include "arduino.h"
#endif

#endif

