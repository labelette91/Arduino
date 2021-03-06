#include <HomeEasyTransmitter.h>
#include "Timer1Transmitter.h"
/*
* timer and interrupts
* Timer1 overflow interrupt example
* more infos: https://oscarliang.com
*/

#define ledPin 13
#define PDATA 3 //pin for data input/output
#define PCLK  0 //pin for clk  input/output

HomeEasyTransmitter easy(PDATA,0,0);

void printBinary ( byte * data , byte pos)
{
  for (byte i = 0; i < pos; ++i)
  {
    byte bt = data[i];
    for (byte b = 0; b < 8; b++)
    {
      if (bt & 0x80)
        Serial.print('1');
      else
        Serial.print('0');
      bt = bt << 1;
      if (b == 3) Serial.print(' ');
      if (b == 7) Serial.print(' ');

    }
  }
}

void printBuffer()
{
  word  i=0 ;

  while(timer1Transmitter.Buffer[i]!=0)
  {
    if ( (i%4)==0 ) {
      Serial.println();
      Serial.print(i/4);
      Serial.print(':' ) ;
      
    }
    Serial.print(timer1Transmitter.Buffer[i] );
    Serial.print(' ' ) ;
    i++;
    
  }

}
#include "hagerdomo.h"
void HagerPrint();
extern byte HagerCmd[];
void setup ()
{
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW );
  pinMode(PDATA, OUTPUT);
  digitalWrite(PDATA, 0 );
  Serial.begin(38400);
  
  timer1Transmitter.SetPin(PDATA,ledPin);
  
  //easy.SetTransmitBuffer(timer1Transmitter.Buffer,1,0x55555555,1);

  delay(1000);

	//printBuffer();

  HagerSetPin (PDATA,ledPin );
  HagerSends1(0x01,1,1 ) ;
  printBuffer();
  HagerPrint();
  printBinary ( HagerCmd ,8 );
  
  delay(5000);

}


void loop()
{
//timer1Transmitter.StartTransmission(4); 
//  delay(1000);
//	HagerSends(0x01,1,1 ) ;
//	 HagerPrint();
//  delay(1000);
//  HagerSends1(0x01,1,1 ) ;
//  HagerPrint();
  delay(1000);
  timer1Transmitter.StartTransmission(1);
  delay(1000);
   

  

}

