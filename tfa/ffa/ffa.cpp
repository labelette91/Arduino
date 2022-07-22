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


#include "../tfaDecoder.h"
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


-1
};




int main()
{

    int i=0;
while (pulse[i]!=-1)
{
			if (tfa3208.nextPulse(pulse[i]*500 + 500 ))
                printf("ok");
printf ("i:%2d p:%d state:%d \n", i, pulse[i],tfa3208.state );
            i++;
}
printf("%s\n",tfa3208.sbits);

//printf ("%s\n",tfa3208.Spaquet.c_str());

for (int i=0;i<tfa3208.total_bits/8;i++ )
        printf ("%d ",tfa3208.data[i]);

for (int i=0;i<tfa3208.total_bits/8;i++ )
        printf ("%02X ",tfa3208.data[i]);

printf("\n");
//printf("%s", tfa3208.Spaquet.substring(0 , 11).c_str() );

printf("Décodage : ");
printf("%d",tfa3208.GetCanal());
printf(" " );
printf("%f",tfa3208.GetTemp());
printf(" " );
printf("%d\n",tfa3208.GetHum());


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
