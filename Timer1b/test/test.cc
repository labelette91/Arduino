// test.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include "deftype.h"
#include "..\hagerdomo.h"

extern byte HagerCmd[];

// #include "managerold.h"
void HagerPrint()
{
	  //print
  for (byte i=0;i<8;i++) 
  {
  	printf("%02X ", HagerCmd[i] );  
  }	
  printf("\n");
}

void HagerSends1(byte id4, byte cmnd, byte NbTime);

int _tmain(int argc, _TCHAR* argv[])
{
  HagerSends(0x55 , 1 , 1 ) ;
  HagerPrint();

  HagerSends1 (0x55 , 1 , 1 ) ;
HagerPrint();

	return 0;
}

