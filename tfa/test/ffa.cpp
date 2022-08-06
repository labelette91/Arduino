// ffa.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#define byte int
#define word int
//#define String std::string

class String: public std::string {
public:
std::string substring(int deb, int end)
{
    return this->substr(deb,end-deb-1) ;
}

String operator =(const char*  n)
{
this->append(n) ;
return *this ;
}
}; 

void PulseLed()
{
}
#include "../../rubicson/VSPDE.h"
#include  "../../Ook_OSV12/reportSerial.h"

#include "../../Ook_OSV12/DecodeOOK.h"
#include "../../libraries/tfa/tfaDecoder.h"
Hideki tfa3208;


word pulse[]= {
0,
0,
1,
0,
0,
1,
0,
0,
0,
0,
1,
0,
0,
0,
0,
0,
0,
0,
0,
1,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
1,
1,
1,
1,
1,
0,
0,
0,
0,
1,
1,
1,
1,
1,
0,
0,
1,
0,
0,
1,
1,
0,
0,
0,
0,
0,
0,
0,
0,
1,
1,
1,
1,
0,
0,
1,
1,
1,
1,
1,
0,
0,
1,
1,
1,
0,
0,
1,
0,
0,
0,
0,
0,
0,
0,
0,
1,
1,
0,
0,
1,
0,
0,
1,
0,
0,
0,
0,
1,
0,
0,
0,


0xFFFF
};



void loop()
{}
void setup()
{

    int i=0;
while (pulse[i]!=0xFFFF)
{
			if (tfa3208.nextPulse(pulse[i]*500 + 500 )){
                printf("ok");
                printf ("i:%2d p:%d state:%d \n", i, pulse[i],tfa3208.state );
                break;
            }
            i++;
}

//printf ("%s\n",tfa3208.Spaquet.c_str());

tfa3208.ReportSerial();

//for (int i=0;i<tfa3208.total_bits/8;i++ )         printf ("%d ",tfa3208.data[i]);
//for (int i=0;i<tfa3208.total_bits/8;i++ )        printf ("%02X ",tfa3208.data[i]);
//printf("\n");
//printf("%s", tfa3208.Spaquet.substring(0 , 11).c_str() );

printf("decodage : ");
printf("%d ",tfa3208.getId());
printf("%d",tfa3208.getChannel());
printf(" " );
printf("%f",tfa3208.getTemperature());
printf(" " );
printf("%d\n",tfa3208.gethumidity());


if ( tfa3208.getId()         != 100       )printf ("error 1");
if ( tfa3208.getChannel()    != 2         )printf ("error 2");
if ((tfa3208.getTemperature()-26.5212)>.01)printf ("error 3");
if ( tfa3208.gethumidity()   !=62         )printf ("error 4");
if ( tfa3208.getBatteryLevel()!= 15       )printf ("error 5");

// 5159052B4FB3 01010001 01011001 00000101 00101011 01001111 10110011

//              01010001 01011001 00000101 00101011 01001111 10110011 
// 010100010101100100000101001010110100111110110011 
   
// 01010001010 1100 10
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
