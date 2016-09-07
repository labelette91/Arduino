#if _MSC_VER 

#include "VSPDE.h"
#endif


#include "fifo.h"
TFifo fifo;

#include "..\DecodeMD230.h"

DecodeMD230 orscV2(1);



#define PORT 2

#define ledPin  13

volatile word pulse;

long int 	LastReceive ;
int 		NbReceive;
word 		Dt;
long int 	NbPulse  ;

int HexDec1digit(char  hexV)
{
  if ((hexV >= '0') && (hexV <= '9')) return (hexV - '0');
  if ((hexV >= 'A') && (hexV <= 'F')) return (hexV - 'A' + 10);
  return 0;
}

int HexDec(char * hexv, int nbdigit)
{

  int res = 0;
  for (int i = 0; i < nbdigit; i++)
  {
    res *= 16;
    res += HexDec1digit(*hexv++);
  }
  return res;
}

void setup () {

/*	fifo.Clear();
	fifo.Put(10,(byte*)"toto");
	fifo.Put(10,(byte*)"toto1");
	fifo.Put(10,(byte*)"toto2");
	while (!fifo.Empty())
	{
  			char message[100];
        byte len;

				//rc->getOokCode(message);
        strncpy((char*) message,(const char*)fifo.Get(len),90 ) ;
				printf("%s",message);
	}
	
*/	
  char * pt = "3A8B0001000000000055";

//unit code     data[3] 
  int UnitCode = HexDec(&pt[3 * 2], 2);

  //switch value     data[4] 
  int OnOff = HexDec(&pt[4 * 2], 2);
  //group value     data[5] 
  int Group = HexDec(&pt[5 * 2], 2);

  // ID data[6..9]
  int CodeId = HexDec(&pt[6 * 2], 8);

	
	  LastReceive = 0 ;
	  NbReceive   = 0;
    
    Serial.begin(38400);
//    Serial.println("\n[ookDecoder]");
    
   // initialize the LED pin as an output:
    pinMode(ledPin, OUTPUT);       
    
    pulse=0;
}

#define    BIT_0  300 , 300 ,
#define    BIT_1  300 , 1300 ,
#define    BITS   300 , 2300 ,

#define    BIT0  300 , 300  , 300 , 1300 ,
#define    BIT1  300 , 1300 ,300 , 300 ,


byte result1[] = {
0xD1,
0x24,
0xF4,
0xEB,
0x0B,
0,
0,
0,
};

int ConfZ1[]  =  {  
3840,
330 ,
240 ,
750 ,
270 ,
750 ,
660 ,
330 ,
240 ,
750 ,
660 ,
330 ,
660 ,
330 ,
660 ,
330 ,
270 ,
750 ,
660 ,
330 ,
660 ,
330 ,
270 ,
720 ,
660 ,
330 ,
660 ,
330 ,
270 ,
720 ,
660 ,
330 ,
660 ,
330 ,
270 ,
720 ,
270 ,
720 ,
270 ,
720 ,
270 ,
720 ,
660 ,
330 ,
270 ,
720 ,
660 ,
330 ,
660 ,
330 ,
270 ,
720 ,
270 ,
720 ,
270 ,
720 ,
660 ,
330 ,
270 ,
720 ,
660 ,
330 ,
270 ,
720 ,
270 ,
720 ,
270 ,
720 ,
660 ,
330 ,
270 ,
720 ,
270 ,
720 ,
3870,


0,
    };

 int ConfZ2[]  = {
1430,350 ,510 ,630 ,550 ,630 ,910 ,270 ,470 ,670 ,910 ,230 ,910 ,310 ,910 ,230 ,630 ,630 ,790 ,390 ,910 ,150 ,590 ,670 ,910 ,230 ,910 ,310 ,470 ,670 ,910 ,230 ,910 ,230 ,550 ,670 ,470 ,670 ,510 ,630 ,550 ,680 ,870 ,270 ,470 ,670 ,910 ,390 ,870 ,190 ,590 ,670 ,510 ,630 ,430 ,750 ,910 ,270 ,470 ,670 ,910 ,230 ,550 ,670 ,470 ,670 ,470 ,670 ,910 ,390 ,390 ,670 ,470 ,670 ,
0
 };

byte result2[] = {  0x2E,0xDB,0x0B,0x14,0x40, 
0,
0,
0,
};

 int ConfZ3[]  = {
1640,150,590,670,510,630,910,270,510,670,910,230,910,230,910,310,470,670,910,230,910,390,390,670,910,230,910,350,430,750,830,350,790,350,430,750,510,670,470,680,510,630,910,270,510,670,910,230,910,230,550,670,470,670,550,710,790,390,390,670,910,350,430,710,430,750,510,670,910,230,550,630,510,670,
000, 	
 	
 	};


int iConf=0;
int jConf=0;

int * config[] = { ConfZ2, ConfZ3 , 0 , 0 };
int * conf    = config[0] ;
byte data = 0;
void setPulse()
{

if ( conf[iConf]!=0)  
{
  pulse = conf[iConf++]  ;
  if (data==0) data=1;else data=0;
}
else 
{
  orscV2.resetDecoder();		
  data = 0;
  iConf=0;
  jConf++;
  conf = config[jConf] ;
    
}
}

char * state [] = {
"UN",
"T0", 
"T1", 
"T2", 
"T3", 
"OK", 
"DO" };


#include <stdlib.h>

void printState()
{
  Serial.print("NbPulse ");
  Serial.print(NbPulse,10);

  Serial.print(" state ");
  Serial.print(state[orscV2.state]); 

  Serial.print(" data:");
  Serial.print(data,10);

  Serial.print(" Bits ");
  Serial.print(orscV2.total_bits,10);

  Serial.print(" code:");
  for (byte j=0;j<SIZE_CODE;j++)
    Serial.print(orscV2.CurCode[j],HEX);
  
  Serial.print(" p:");
  Serial.print(conf[iConf-1],10);

  Serial.println();
    

}
void loop () {
    setPulse();
    word p = pulse;
    pulse = 0;
    if (conf==0)
      exit(0);
    if (p != 0) {
        NbPulse++;
        printState();    
        if (orscV2.nextPulse(p,data))
        {
					for (int i = 0; i < orscV2.getBytes(); i++)
						printf("%02X ", orscV2.getData()[i]);

					  printf (" Home: ");
            for (int j=0;j<SIZE_CODE;j++)
					    printf ("%02X ",orscV2.Code[j]);
            printf("\n");

						Serial.println();
						if (TEST_CODE(result2,orscV2.Code) )
							Serial.println(" OK");
						else
						Serial.println(" KO");

            orscV2.ReportSerial();
						NbReceive ++ ;
            NbPulse=0 ;
           orscV2.resetDecoder();		
                
        }
    }

}


