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


#include "../OOKDecoder.h"
Hideki orscV2;


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


-1
};



byte GetHum (byte* data)
{
	byte  iHum; // humidité
	
	//hum 8 bits bits 34 .. 41 6 bit data[4] + 2 bits data[5]
	iHum = data[4] & 0x3F ;
	iHum <<= 2 ;
	iHum += data[5] >> 6 ;
    return iHum;
}	
	
byte GetCanal (byte* data)
{
/*	iCanal = Bin2Dec(Spaquet.substring(19, 22));
    fTemp = (Bin2Dec(Spaquet.substring(22, 34)) - 720) * 0.0556;
    fTemp *= 10;
    fTemp = int(fTemp + 0.5);
    fTemp /= 10;
    iHum = Bin2Dec(Spaquet.substring(34, 42));
*/	
	byte iCanal ; //le canal du satellite

	//bit 19 a 21
	iCanal  = data[2] >> 2;

    return iCanal & 0x3 ;
}	
float GetTemp (byte* data)
{
	float fTemp; // Température

	//temp bit 22 a 33 : 12 bit en faraneight :  2 bit  data[2] + 8 bits  data[3] + 2 bits  data[4]
	word 
	Temp   = data[2] & 0x3 ;
	Temp <<=  8 ;
	Temp += data[3] ;
	Temp <<=  2 ;
	Temp += ( data[4] >>6 ) ;

	//convert T = (X - 720) * 0.0556
	fTemp = (Temp-720)* 0.0556;
    return fTemp;	
}	
	
	


int main()
{

    int i=0;
while (pulse[i]!=-1)
{
			if (orscV2.nextPulse(pulse[i]*500 + 500 ))
                printf("ok");
printf ("i:%2d p:%d state:%d \n", i, pulse[i],orscV2.state );
            i++;
}
printf("%s\n",orscV2.sbits);

printf ("%s\n",orscV2.Spaquet.c_str());

for (int i=0;i<orscV2.total_bits/8;i++ )
        printf ("%d ",orscV2.data[i]);

for (int i=0;i<orscV2.total_bits/8;i++ )
        printf ("%02X ",orscV2.data[i]);

printf("\n");
printf("%s", orscV2.Spaquet.substring(0 , 11).c_str() );

printf("Décodage : ");
printf("%d",GetCanal(orscV2.data));
printf(" " );
printf("%f",GetTemp(orscV2.data));
printf(" " );
printf("%d\n",GetHum(orscV2.data));


// 01010001 01011001 00000101 00101011 01001111 10110011 
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
