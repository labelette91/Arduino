#ifndef _DecodeHomeEasy_
#define _DecodeHomeEasy_

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>            //assumes Arduino IDE v1.0 or greater
#else
#include "deftype.h"
#endif

#include "DecodeOOK.h"

#define NBBIT  32
#define PULSE_HIGH 400
#define PULSE_ONE  1300
#define PULSE_ZERO 300
#define PULSE_SYNCHRO 2000

#define TOLERANCE 400

#define HOMESWITCH_ID0 0x3B
#define HOMESWITCH_ID1 0x80

//#define _debug

/* bit 1 : |-|____ : short Pulse One / long  Pulse Low  */
/* bit 0 : |-|_    : short Pulse One / short Pulse Low  */
/*

https://landru29.github.io/arduino-et-le-protocole-homeeasy/index.html

Principe du Protocole HomeEasy
 
Le protocole de transmission RF HomeEasy est utilis� par diff�rents fabricants de syst�mes domotiques tel que Chacon, NEXA, KlikAanKlikUit, HomeEasy, Intertechno ou D�wi.
Ce protocole utilise bien �videmment la fr�quence 433MHz r�glement�e par l�Union internationale des t�l�communications.
La trame est compos�e de 32 bits (ON/OFF) ou 36 bits (DIM). Avant les donn�es, il y a un front haut de 275us puis un front bas de 2675us.
Un 0 est encod� par un front haut de 275us puis un front bas de 240us, et un 1 est encod� par un front haut de 275us puis un front bas de 1300us.
Les bits de donn�es sont encod�s sous la forme 0 = 01 et 1 = 10.

Trame :
01101001100101101001011010100101010110101010101010100110010110010

Donn�es :
0 1 1 0 1 0 0 1 1 0 0 1 1 1 0 0 0 0 1 1 1 1 1 1 1 1 0 1 0 0 1 0 0

Les donn�es correspondent aux informations suivantes :

    Bits 0 � 25 : Num�ro identifiant l��metteur (num�ro unique qui peut �tre g�n�r� al�atoirement)
    Bit 26 : Flag Group
    Bit 27 : ON/OFF
    Bits 28 � 31 : Code device (un m�me id peut avoir 16 code device)
    Bits 32 � 36 : Dim level (facultatif)

*/


#define TEST_PULSE(WIDTH,PULSE_LEN,TOL)((WIDTH> (PULSE_LEN-TOL))&&(WIDTH<(PULSE_LEN+TOL)))

class DecodeHomeEasy  : public DecodeOOK{
//class DecodeHomeEasy {
public:
    byte  MaxCode , NbCodeOk , lastBit , total_bit  ;
//    byte total_bits, state, MaxCode , NbCodeOk , lastBit , pos, data[10]; ;
    unsigned long CurCode , Code , tmpCode, lastCode;
    unsigned long LastReceived ; 

public:
    enum { UNKNOWN, T0, T1, T2, T3, OK, DONE };

    const byte* getData (byte& count) const ;
    const byte* getData () const ;
     byte getBytes ()  ;

    void resetDecoder () ;
    
    DecodeHomeEasy (byte pMaxCode=5) ;
    bool nextOnePulse (word pWidth , byte data)   ;
    
  char decode (word width , byte BitData)   ;
  void report();
 };

#endif