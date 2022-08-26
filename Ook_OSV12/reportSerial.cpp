#include <stdio.h>
#include <Arduino.h>

typedef enum {
REPORT_DOMOTIC,
REPORT_SERIAL       ,
SERIAL_DEBUG 
} T_REPORTTYPE;

void setReportType(T_REPORTTYPE pReportType );
T_REPORTTYPE getsetReportType();
bool isReportSerial();
        
#ifdef WIN32
void registerStdout() {};
#else

static FILE uartout = {0} ;

static int uart_putchar (char c, FILE *stream)
{
  Serial.write(c) ;
  return 0 ;
}

void registerStdout() {
  fdev_setup_stream (&uartout, uart_putchar, NULL, _FDEV_SETUP_WRITE);
  stdout = &uartout ;
}
#endif

static T_REPORTTYPE ReportType =  REPORT_DOMOTIC ;

void setReportType(T_REPORTTYPE pReportType )
{
    ReportType = pReportType ;
}

T_REPORTTYPE getsetReportType()
{
    return ReportType;
}
bool isReportSerial()
{
    return ReportType !=  REPORT_DOMOTIC ;
}


char DectoHex(byte v)
{
    if (v<=9)
        return (v+'0');
    else
        return (v+'A'-10);
}

void printBinary ( byte * data , byte pos, byte space  )
{
    register byte bl = 0;
	for (byte i = 0; i < pos; ++i)
	{
		byte bt = data[i];
		for (byte b = 0; b < 8; b++)
		{
            if ( (bl++%space) == 0 )Serial.print(' ');
			if (bt & 0x80)
				Serial.print('1');
			else
				Serial.print('0');
			bt = bt << 1;
			//if (b == 3) Serial.print(' ');
			//if (b == 7) Serial.print(' ');
		}
	}
}

//sump hex
void printHexa ( byte * data, byte pos)
{

for (byte i = 0; i < pos; ++i) {
	Serial.print(DectoHex( data[i] >> 4 ));
	Serial.print(DectoHex(data[i] & 0x0F));
}
}

void printTab(byte tab, byte n)
{
	tab = tab - n;
	while (tab>0 ) {
		Serial.print(' ');
		tab--;
	}

}

