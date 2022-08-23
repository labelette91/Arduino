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
 
char * Rubicson1  = "540 1890 540 3780 540 1890 540 3780 540 3780 540 3780 540 3780 540 3780 540 1890 540 1890 540 3780 540 4050 540 3780 540 1890 540 1890 540 1890 540 1890 540 1890 540 1890 540 1890 540 3780 540 3780 540 1890 540 1890 540 4050 540 3780 540 1890 540 4050 540 1890 540 1890 540 4050 540 1890 540 3780 540 1890 540 3780 540 3780 540 9180";

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




#include "Fifo.h"
extern TFifo  fifo;

//duree = coefA * duree + float coefB 
void testOOK (char* TestString ,TPulses* TestPulse, float coefA=1.0 , float coefB=0   )
{
    TPulses Pulse ;
    TPulses* LPulse = & Pulse ;
    unsigned int PulseNb=0;

    if (TestString != 0)
        readPulse(TestString,LPulse);
    else
        LPulse = TestPulse;
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


int main(int, char**)
{
//    testOOK (Rubicson1 , 0 ) ;
    testOOK (0         , &TfaPulse , 500 , 500 ) ;

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


