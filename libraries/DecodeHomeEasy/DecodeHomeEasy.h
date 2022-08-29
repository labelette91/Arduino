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
 
Le protocole de transmission RF HomeEasy est utilisé par différents fabricants de systèmes domotiques tel que Chacon, NEXA, KlikAanKlikUit, HomeEasy, Intertechno ou Düwi.
Ce protocole utilise bien évidemment la fréquence 433MHz réglementée par l’Union internationale des télécommunications.
La trame est composée de 32 bits (ON/OFF) ou 36 bits (DIM). Avant les données, il y a un front haut de 275us puis un front bas de 2675us.
Un 0 est encodé par un front haut de 275us puis un front bas de 240us, et un 1 est encodé par un front haut de 275us puis un front bas de 1300us.
Les bits de données sont encodés sous la forme 0 = 01 et 1 = 10.

Trame :
01101001100101101001011010100101010110101010101010100110010110010

Données :
0 1 1 0 1 0 0 1 1 0 0 1 1 1 0 0 0 0 1 1 1 1 1 1 1 1 0 1 0 0 1 0 0

Les données correspondent aux informations suivantes :

    Bits 0 à 25 : Numéro identifiant l’émetteur (numéro unique qui peut être généré aléatoirement)
    Bit 26 : Flag Group
    Bit 27 : ON/OFF
    Bits 28 à 31 : Code device (un même id peut avoir 16 code device)
    Bits 32 à 36 : Dim level (facultatif)

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