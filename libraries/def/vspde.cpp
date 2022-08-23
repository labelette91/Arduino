//--------------------------------------------------------------------
// Arduino Console Stub
//--------------------------------------------------------------------

#if _MSC_VER 

#include "VSPDE.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <string>



extern void setup();
extern void loop();

Print Serial;

//--------------------------------------------------------------------
// Timers
//--------------------------------------------------------------------
void init(void)
{}
void initVariant(void)
{}

void pinMode(uint8_t, uint8_t)
{}
void digitalWrite(uint8_t, uint8_t)
{}
int digitalRead(uint8_t)
{
	return 0;
}
int analogRead(uint8_t)
{return 0;}
void analogReference(uint8_t mode)
{}
void analogWrite(uint8_t, int)
{}

unsigned long millis(void)
{
//	return GetTickCount();
  return 1;
}
unsigned long micros(void)
{
return 1;
}
void delay(unsigned long delayms)
{	unsigned long u = millis() + delayms;
//	while (u > millis())		;
}
void delayMicroseconds(unsigned int us)
{}
unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout)
{
return 0;
}
unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout)
{
return 0;
}

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
{}
uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder)
{
return 0;
}

void attachInterrupt(uint8_t, void (*)(void), int mode)
{}
void detachInterrupt(uint8_t)
{}

 void cli(){};
 void sei(){};


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
	printf("%0X ", value);
  else
	printf("%d", value);

}

void CSerial::print(int value)
{
		printf("%d", value);
}
void CSerial::print(byte value)
{
		printf("%d", value);
}
void CSerial::print(unsigned long  value)
{
		printf("%d", value);
}
void CSerial::print(float value)
{
		printf("%f", value);
}

void CSerial::println()
{
	printf("\r\n");
}

void CSerial::println(const char *pString)
{
	printf("%s\r\n", pString);
}

void CSerial::println(int value, int)
{
	printf("%d\r\n", value);
}
void CSerial::println(byte value)
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


#include <vector>

 typedef std::vector<int> TPulses ;

/// PulseString = "540 1890 540  " or "540;1890;540 "
 // lit les valeur de pulse a partir chaine 
 void readPulse( char* PulseString ,  TPulses* Pulse  )
{
  int nb=0;

  std::string list[1000];
  Split ( PulseString  , " ;" , "" , true , list ) ;
  while (list[nb]!="")
  {
    int pulse = atoi (list[nb].c_str())  ;
    Pulse->push_back ( pulse );
    nb++;
  }
}

 void convertdPulse( TPulses* Pulse , float coefA = 1 , float coefB=0  )
{
  int nb=0;

  for (unsigned int i = 0 ; i<Pulse->size() ;i++ )
  {
    int pulse =(*Pulse)[i] * coefA + coefB ;
    (*Pulse)[i] =  ( pulse );
  }
}

//--------------------------------------------------------------------
// Main
//--------------------------------------------------------------------
 
char * RubicsonPulse  = "540 1890 540 3780 540 1890 540 3780 540 3780 540 3780 540 3780 540 3780 540 1890 540 1890 540 3780 540 4050 540 3780 540 1890 540 1890 540 1890 540 1890 540 1890 540 1890 540 1890 540 3780 540 3780 540 1890 540 1890 540 4050 540 3780 540 1890 540 4050 540 1890 540 1890 540 4050 540 1890 540 3780 540 1890 540 3780 540 3780 540 9180";

TPulses TfaPulse= {
0  ,
0  ,
1  ,
0  ,
0  ,
1  ,
0  ,
0  ,
0  ,
0  ,
1  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
1  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
1  ,
1  ,
1  ,
1  ,
1  ,
0  ,
0  ,
0  ,
0  ,
1  ,
1  ,
1  ,
1  ,
1  ,
0  ,
0  ,
1  ,
0  ,
0  ,
1  ,
1  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
1  ,
1  ,
1  ,
1  ,
0  ,
0  ,
1  ,
1  ,
1  ,
1  ,
1  ,
0  ,
0  ,
1  ,
1  ,
1  ,
0  ,
0  ,
1  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
1  ,
1  ,
0  ,
0  ,
1  ,
0  ,
0  ,
1  ,
0  ,
0  ,
0  ,
0  ,
1  ,
0  ,
0  ,
0  ,


0xFFFF
};


/* pulse in 100micros unit 
short : 2000..2200 micros
long  : 4000..4200 micros
end   :8700..8800    
*/
char* OtioPulse = 
"   42 21 20 41 21 41 21 41 21 21 20 42 21 41 41 20 21 41 21 20 42 21 21 40 "
"87 41 21 20 42 20 42 20 42 21 21 20 42 20 42 41 21 20 42 20 21 41 21 20 40 "
"87 41 20 21 43 20 41 21 43 20 21 21 42 20 42 41 22 20 41 21 21 41 21 21 40 "
"87 41 21 20 41 19 42 20 42 20 22 20 42 19 42 41 21 19 42 20 21 41 22 20 39 "
"88 41 20 21 42 20 41 20 42 21 20 21 42 20 41 42 21 20 58 21 20 42 20 21 40 "
"87 41 20 21 41 20 41 22 41 20 21 21 41 21 41 42 20 20 41 22 20 42 21 21 40 "
"87 42 20 21 41 21 41 21 40 21 20 21 41 22 41 42 20 21 41 21 19 42 20 21 39 "
"87 42 20 21 41 21 41 21 41 21 20 21 41 21 40 42 21 21 41 21 20 42 20 21 40 "
"87 41 20 21 41 21 41 22 41 21 21 21 41 21 41 42 20 21 41 22 20 42 21 21 40 "
"87 42 20 21 41 21 41 21 40 21 20 21 41 22 41 42 20 21 41 21 19 42 20 21 39 "
"87 42 20 21 41 21 41 42 41 22 20 21 41 21 41 41 19 22 41 21 21 43 20 21 40 "
"87 42 20 21 41 21 40 21 41 21 20 22 41 21 41 42 20 21 40 21 20 42 20 22 40 "
"87 42 21 21 41 21 41 21 41 21 21 22 40 21 41 43 20 21 41 21 20 42 21 21 39 "
"87 42 20 21 41 22 41 21 41 22 20 21 41 21 41 41 19 22 41 21 21 43 20 21 40 "
"87 42 20 21 41 21 40 21 41 21 19 22 41 21 41 42 20 21 40 21 20 41 20 20 40 "
"87 42 21 21 41 21 41 21 41 21 21 21 40 21 41 43 20 21 41 21 20 42 21 21 56 "
"87 41 20 21 41 20 41 21 41 20 20 21 42 20 42 42 21 20 41 21 20 41 20 21 39 "
"87 40 21 20 41 21 42 20 41 20 21 20 41 21 42 41 20 21 42 20 20 40 21 20 40 "
"87 42 20 20 42 21 41 20 42 21 20 20 42 20 41 41 21 21 41 20 21 42 20 20 41 "
"87 41 20 21 43 20 41 21 42 20 20 21 43 20 42 41 21 20 41 21 21 41 20 21 40 "
"87 40 21 20 41 20 42 20 41 20 21 20 41                                     "
;


//OSV2 6 Np:1349   1A2D405E302350463E9C[THGR228N] Id:5E Ch:3 T:23.3 233 65% bat:15
char* OregonTempPulse = 

//"1551 28 26 332 32 21 217 25 21 34 29 22 20 24 34 134        "
//"29 65 27 87 95 51 27 317 21 667 24 20 26 102 62 44          "
//"40 29 38 37 174 22 64 1564 25 32 27 20 34 56 29 28          "
//"34 36 37 31 41 26 31 24 32 38 31 42 26 31 23 23             "
//"53 36 31 110 20 65 73 23 52 97 456 23 183 30 24 63          "
//"20 45 59 27 28 24 67 71 59 68 756 37 864 22 80 26           "
//"21 26 369 24 62 36 44 38 24 23 41 40 23 31 52 162           "
//"33 22 32 280 24 427 72 29 27 58 56 25 75 82 84 74           "
//"43 32 25 671 34 670 25 22 344 26 32 28 24 93 23 27          "
//"20 46 61 36 49 42 162 64 39 32 24 572 23 24 22 30           "
//"89 24 86 24 26 30 66 34 47 171 46 57 60 1210 38 32          "
//"26 20 29 20 21 20 22 27 22 24 379 48 42 36 20 37            "
//"28 100 34 46 139 62 59 29 653 28 33 47 32 34 50 36          "
//"38 35 60 96 32 38 33 1323 29 37 72 22 32 45 20 28           "
//"30 35 26 30 27 32 32 21 32 302 38 105 90 20 292 22          "
//"596 34 30 199 119 67 160 31 46 832 28 26 23 147 24 148      "
//"557 26 27 26 26 168 49 188 26 108 21 23 317 61 20 61        "
//"76 28 23 691 24 172 58 194 29 29 32 496 26 48 227 27        "
//"27 24 30 27 46 173 21 24 35 163 32 33 578 135 22 335        "
//"54 33 33 20 38 22 26 838 21 61 20 367 116 78 37 73          "
//"175 72 1070 38 63 35 22 37 21 451 25 35 34 30 23 28         "
//"34 46 28 32 48 34 38 22 32 59 26 40 29 162 37 20            "
//"53 27 368 21 290 83 74 27 39 55 50 60 64 1122 32 220        "
//"61 36 22 396 26 24 33 25 28 32 34 29 60 22 31 27            "
//"28 52 35 88 21 28 78 30 36 32 75 24 1204 27 31 54           "
//"88 48 38 43 20 40 116 34 21 34 55 185 420 21 986 42         "
//"26 26 70 23 104 40 35 28 20 34 20 133 38 61 50 31           "
"39 81 128 30 41 34 26 497 114 81 111 83 112 83 111 84       "
"109 86 108 87 107 88 106 88 106 89 106 88 106 89 106 89     "
"105 90 105 89 105 89 105 41 56 90 56 41 104 41 56 90        "
"55 41 104 91 104 42 55 91 103 91 104 91 54 42 103 43        "
"54 92 54 43 102 92 102 44 53 93 52 44 101 44 52 94          "
"100 94 100 96 99 96 98 97 97 98 96 100 94 101 45 52         "
"92 55 42 104 90 104 42 55 91 103 92 101 93 101 94 52        "
"45 100 46 51 94 52 46 100 95 99 95 99 96 99 96 99           "
"47 50 96 99 95 50 47 99 96 98 47 50 96 99 96 50             "
"47 98 97 98 96 98 47 50 96 50 47 98 96 98 96 98             "
"97 98 96 98 96 98 48 50 96 49 48 98 48 49 96 50             "
"47 98 96 98 48 49 96 98 96 50 48 98 96 98 96 98             "
"48 49 96 49 48 98 97 98 48 49 96 98 96 98 96 98             "
"96 98 97 49 47 98 96 98 96 98 96 98 48 49 96 98             "
"96 98 96 49 48 98 96 98 48 49 96                            " 
;


//OSV2 16 Np:552    3A80018201000000801F00[CMR180,...] Id:3A80 ,power:386 ,total:-2147483648 ,total kWh:-596
char* OregonPowerPulse = 
"23 34 158 21 232 29 33 256 20 37 92 138 22 21 133 21             "
"29 20 294 24 24 20 383 26 24 39 38 22 24 30 52 23                "
"117 96 111 97 110 98 108 99 108 100 108 100 107 100 107 100      "
"107 101 106 101 106 101 106 101 106 102 106 102 106 102 105 50   "
"54 101 54 49 105 50 54 102 54 49 105 102 105 102 105 50          "
"53 102 105 102 105 102 105 102 105 102 104 103 104 103 103 104   "
"103 105 51 53 101 107 100 55 48 107 99 109 98 110 96 112         "
"95 112 95 113 95 111 97 110 45 58 97 58 46 109 98 109            "
"98 109 99 108 100 107 48 57 98 109 98 57 47 107 100 107          "
"100 107 100 107 100 107 100 107 100 107 100 107 100 108 99 108   "
"100 107 101 106 101 107 100 107 101 108 100 107 100 107 100 107  "
"100 108 100 106 101 107 100 107 100 107 101 107 100 107 100 106  "
"101 107 100 108 100 107 100 106 101 106 101 106 101 107 100 107  "
"101 107 100 107 100 107 100 107 48 56 100 108 100 108 99 108     "
"100 108 100 108 100 56 48 107 101 107 100 107 100 108 100 107    "
"100 107 100 107 101 107 100 107 100 107 100 107                  "
;


// OTIO 29 921C89 T:28.9 Id:92 Bat:15
char * OtioPulse2 =
"941 1004 485 476 993 484 477 984 481 488 993 484 461 1008 485 480       "
"993 468 477 1000 8216 361 2048 397 2028 413 2040 393 4224 425 2016 425  "
"2004 437 4068 453 7948 469 3932 473 1980 477 1968 469 3916 473 1968 473 "
"1972 465 3928 481 1956 481 1964 477 1964 473 1976 481 3900 489 4160 485 "
"3900 493 1944 493 1948 489 4156 493 1948 489 1948 489 1964 493 4136 497 "
"1940 497 1960 493 4004 497 7904 513 3912 513 1924 505 1936 509 3896 505 "
"1936 501 1940 497 3888 505 1948 505 1936 501 1944 497 1940 493 3908 501 "
"4128 501 3900 501 1940 501 1940 497 4148 497 1948 493 1940 497 1944 489 "
"4152 497 1944 493 1948 493 4020 497 7904 513 3908 513 1932 497 1940 501 "
"3888 505 1940 493 1960 497 4132 497 1944 493 1960 493 1948 489 1952 485 "
"3900 493 4152 493 3888 501 1956 497 1944 497 4132 497 1960 493 1944 493 "
"1948 493 4152 493 1944 497 1940 497 4004 497 7908                       "
;



#include "Fifo.h"
extern TFifo  fifo;

//duree = coefA * duree + float coefB 
void testOOK (TPulses* TestPulse, float coefA=1.0 , float coefB=0   )
{
    unsigned int PulseNb=0;

	setup();
	for(;;)
	{
		if (_kbhit())
		{
			Serial._append((char)_getch());
		}
		loop();
        if (PulseNb<TestPulse->size())
        {
            int pulse =(*TestPulse)[PulseNb] * coefA + coefB ;
            fifo.put(pulse);
            PulseNb++;
        }
        else
            break;
	}
}

void testOOK (char* TestString , float coefA=1.0 , float coefB=0   )
{
    TPulses Pulse ;

    readPulse(TestString,&Pulse);
    testOOK (&Pulse, coefA , coefB   );

}

int main(int, char**)
{
//    testOOK (RubicsonPulse  ) ;
//    testOOK (&TfaPulse , 500 , 500 ) ;
    testOOK (OtioPulse2 ,  1 , 0   ) ;
//    testOOK (OregonTempPulse ,  10 , 1   ) ;
//    testOOK (OregonPowerPulse ,  10 , 1   ) ;
    
    
}

#endif

 //
// découpage de ligne
//
char *decoupe(char *ligne, char *mot, char *sep)
{
    /* saute le separateur */
    while((*ligne!=0) && (strchr(sep, *ligne)))
        ligne++;
    /* recupere le mot */
    while((*ligne!=0) && ( !strchr(sep, *ligne)))
        *mot++ = *ligne++;
    *mot= 0;
    /* saute le separateur */
    while((*ligne!=0) && (strchr(sep, *ligne)))
        ligne++;
    return(ligne);
}
//
// découpage de ligne
//
//ligne ligne a decoupe
//mot   : mot decoupe
//separator : liste des carateres separateurs
//concatenator : liste des carateres qui concatene example 
//  si = ''   pour le decoupage de  ;"un;deux";;  retourne '','"un',deux"',''
//  si = '"'  pour le decoupage de  ;"un;deux";;  retourne '','un;deux'   ,''
//  si = '[]' pour le decoupage de  ;[un;deux];;  retourne '','un;deux'   ,''

// AllowDuplicateSeparator = true : un seul carateres separateurs est pris en compte,utiliser
//       si=true       pour le decoupage d'une ligne csv : ;un;deux;;  retourne "","un","deux",""
//       si=false      pour le decoupage d'une ligne csv : un;deux;;  retourne    "un","deux"
//MotLen : taille  max de mot

char *decoupe(char *ligne, char *mot, int MotLen , char *separator, char * concatenator , bool AllowDuplicateSeparator)
{
    char * sep = separator ;
    int nbCar=0;


    /* saute les separateurs de debut de ligne */
    if (AllowDuplicateSeparator)
            while((*ligne!=0) && (strchr(sep, *ligne)))
                ligne++;

    /* recupere le mot */
    while((*ligne!=0) && ( !strchr(sep, *ligne)))
    {
        //si concatenator carateres
        if ( strchr(concatenator, *ligne) )
        {
            sep = concatenator;
            ligne++;
        }
        else
        {
            //test si la taiile max a ete atteinte
            if (nbCar<MotLen)
            {
                *mot++ = *ligne++;
                nbCar++;
            }
        }
    }
    *mot= 0;

    //si concatenator carateres
    if (*ligne!=0)
        if ( strchr(concatenator, *ligne) )
        {
            sep = separator;
            ligne++;
        }

    if (AllowDuplicateSeparator)
    {
            /* saut de tous les separateur */
            while((*ligne!=0) && (strchr(sep, *ligne)))
                ligne++;
    }
    else
         /* saut du separateur */
         ligne++;


    return(ligne);
}
//
// découpage de ligne d'un csv
//
char *decoupecsv(char *ligne, char *mot, int MotLen , char *sep)
{
    int nbCar=0;
    MotLen--;
    /* test si fin de ligne \r \n 0*/
    if ( ( *ligne ==  '\r' ) || ( *ligne ==  '\n' ) )
    {
        *mot= 0;
        /* force fin de ligne ci \r \n */
        *ligne=0;
    }
    else
    /* test si pas de valeur : separateur */
    if (strchr(sep, *ligne))
    {
        *mot= 0;
        /* saute le separateur */
        ligne++;
    }
    else
    {
        /* recupere le mot */
        while((*ligne!=0) && ( !strchr(sep, *ligne)) && (nbCar<MotLen) )
        {
            *mot++ = *ligne++;
            nbCar++;
        }
        *mot= 0;
        /* saute le separateur */
        if (*ligne)
           ligne++;
    }
    return(ligne);
}



 void Split ( char * ptligne , char * separateur , char * concatenator , bool AllowDuplicateSeparator , std::string list[] )
{
     char mot[1024];
     int nb=0;
     {

       while (*ptligne)
       {
         if (AllowDuplicateSeparator)
            ptligne = decoupe(ptligne , mot  ,sizeof(mot), separateur , concatenator, true );
         else
            ptligne = decoupecsv(ptligne , mot  , sizeof (mot), separateur );

         list[nb++] =  (mot);
       }
     }
}

 byte Header[] = {
21 ,	//0 : 1 / 208
22 ,	//1 : 0 / 212
0 };


