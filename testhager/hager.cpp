#include "arduino.h"
#include "domotic.h"
#include <avr/pgmspace.h>

extern byte  Header[];

//comand 
/*
byte cmdZ1eco   [] = {0x07,0x04,0xFA,0x00,0x02,0x20,0x8B,0x4E};
byte cmdZ1confor[] = {0x07,0x04,0xFA,0x00,0x02,0x20,0x88,0x51};
byte cmdZ2eco   [] = {0x07,0x04,0xFA,0x00,0x02,0x20,0x93,0x46};
byte cmdZ2confor[] = {0x07,0x04,0xFA,0x00,0x02,0x20,0x90,0x49};
byte cmdZ3eco   [] = {0x07,0x04,0xFA,0x00,0x02,0x20,0xA3,0x36};
byte cmdZ3confor[] = {0x07,0x04,0xFA,0x00,0x02,0x20,0xA0,0x39};
byte cmdZ1Hgel  [] = {0x07,0x04,0xFA,0x00,0x02,0x20,0xCC,0x0D};
byte cmdZ2Hgel  [] = {0x07,0x04,0xFA,0x00,0x02,0x20,0xD4,0x05};
byte cmdZ1config[] = {0x07,0x04,0xFA,0x00,0x02,0x05,0x01,0xF3};
byte cmdZ2config[] = {0x07,0x04,0xFA,0x00,0x02,0x05,0x02,0xF2};
byte cmdZ3config[] = {0x07,0x04,0xFA,0x00,0x02,0x05,0x04,0xF0};
     
byte cmdZ6eco[]    = {0x87,0x04,0xFA,0x00,0x02,0x20,0xA3,0xB6 };
byte cmdZ6confor[] = {0x87,0x04,0xFA,0x00,0x02,0x20,0xA0,0xB9 };
byte cmdZ6config[] = {0x87,0x04,0xFA,0x00,0x02,0x05,0x04,0x70 };

byte Addr2   [] = {0x07,0x04,0xF1,0x00,0x02 };
byte Addr3   [] = {0x07,0x04,0xF2,0x00,0x02 };

byte Z3eco   [] = {0x20,0xA3};
byte Z3confor[] = {0x20,0xA0};
byte Z3Hgel  [] = {0x20,0xE4};
byte Z3config[] = {0x05,0x04};

*/

byte Addr1   [] = {0x07,0x04,0xFA,0x00,0x02 };

byte Z1eco   [] = {0x20,0x8B};
byte Z1confor[] = {0x20,0x88};
byte Z1Hgel  [] = {0x20,0xCC};
byte Z1config[] = {0x05,0x01};

byte HagerChk ;

byte HagerCmd[8];
byte IdCmd;

#define pData   3
#define pLedPin 9 //pin for data input/output
#define HAGER_DELAY 210

//0 to 1 transition
//send 1
void HagerSendHigh()
{
digitalWrite(pData, LOW);
delayMicroseconds(HAGER_DELAY)  ;
digitalWrite(pData, HIGH);
delayMicroseconds(HAGER_DELAY)  ;
}
//1 to 0 transition
//send 0
void HagerSendLow()
{
digitalWrite(pData, HIGH);
delayMicroseconds(HAGER_DELAY)  ;
digitalWrite(pData, LOW);
delayMicroseconds(HAGER_DELAY)  ;
}
//send bit
void HagerSendBit(bool bit)
{
	if (bit)
		HagerSendHigh();
	else
		HagerSendLow();
}
//send byte 8 bits 
void HagerSendByte(byte  bite )
{
	HagerCmd[IdCmd++]=bite;
	HagerChk += bite;
	for (char i=7;i>=0;i--)
	{
		HagerSendBit ( bite & ( (byte)1 << i ) );
	}
}
//send array of byte 8 bits 
void HagerSendBytes(byte  * bite , byte nb )
{
	for (byte i=0;i<nb;i++)
	{
		HagerSendByte ( *bite++ ) ;
	}
}

//send Pulse = array duration in 10 micros
void HagerSendOne(byte * Pulse  )
{
  byte i=0;
  word delay;
  if (pLedPin) digitalWrite(pLedPin, HIGH);
  while (pgm_read_byte(Pulse) != 0x00)
  {
    if (i%2==0) 
      //output = 1 
      digitalWrite(pData, HIGH);
    else
      //output = 0
      digitalWrite(pData, LOW);
    delay = pgm_read_byte(Pulse++);
    delay = delay *10 ; 
    delayMicroseconds(delay)  ;
    i++;
  }
  digitalWrite(pData, LOW);

  if (pLedPin) digitalWrite(pLedPin, LOW);
}


//cmnd = 0 : eco
//cmnd = 1 : conf
//cmnd = 2 : hors gel
//cmnd = 3 : arret
//cmnd = 4 : configuration t

void HagerSend ( byte * addr , byte cmnd )
{
  cli();
  HagerChk=0;
  IdCmd=0;
  HagerSendOne(Header );
  HagerSendBytes(addr , 5  ) ;
  if (cmnd==0)
		HagerSendBytes(Z1eco , 2  ) ;
  else if (cmnd==1)
		HagerSendBytes(Z1confor , 2  ) ;
  else if (cmnd==2)
		HagerSendBytes(Z1Hgel , 2  ) ;
  else if (cmnd==3)
		HagerSendBytes(Z1Hgel , 2  ) ;
  else if (cmnd==4)
		HagerSendBytes(Z1config , 2  ) ;

	//compute checksum
	HagerChk = ~HagerChk + 1 ;
		
	HagerSendByte(HagerChk);
  sei();

}

// #include "managerold.h"
void HagerPrint()
{
	  //print
  for (byte i=0;i<8;i++) 
  {
  	if (i%4==0)Serial.print(' ');
  	if (HagerCmd[i]<16) Serial.print('0');
  	Serial.print(HagerCmd[i],HEX);  
  }	
  Serial.println();  
}
void ManageHager(byte id4 , byte cmnd)
{
	
	//set adresse
	Addr1[2] = 0xF0 + id4 ;
  HagerSend ( Addr1 , cmnd );
	delay(10);
  HagerSend ( Addr1 , cmnd );
	delay(10);
}


void ManageHager( )
{
	ManageHager(Cmd.LIGHTING2.id4,Cmd.LIGHTING2.cmnd);
}

