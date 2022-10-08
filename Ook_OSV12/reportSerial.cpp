#include <stdio.h>
#include <Arduino.h>
#include  "reportSerial.h"

void printRSSI();

#if defined(__AVR_ATmega328__) || defined(__AVR_ATmega2560__)


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
#else
void registerStdout() {};

#endif

static T_REPORTTYPE ReportType =  REPORT_DOMOTIC ;

void setReportType(T_REPORTTYPE pReportType )
{
    ReportType = pReportType ;
}

T_REPORTTYPE getReportType()
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

extern word 	NbPulsePerSec ;;

void reportPrintHeader()
{
    Serial.print(' ');
    Serial.print(millis() / 1000);

    Serial.print(" Np:");
    byte nb = Serial.print(NbPulsePerSec);
    printTab(6, nb);
    Serial.print(' ');
}
void reportPrint(const char * mes)
{
     if (isReportSerial()) Serial.print(mes); 
}
void reportSerial(const char* Name, byte id1, byte id2, byte bateryLevel, int temp, byte hum, word power, unsigned long totalpower, word pressure, word PressureSeaLevel, word Rain ,  byte* data, byte pos) {

    printTab(TAB,Serial.print(Name)) ;
    reportPrintHeader();

    Serial.print(" Id1:");
    Serial.print(id1, HEX);
    Serial.print(" Id2:");
    Serial.print(id2,HEX);
    Serial.print(" Bat:");
    Serial.print(bateryLevel);

    if (temp != INVALID_TEMP)
    {
        Serial.print(" T:");
        Serial.print(temp/10);
        Serial.print(".");
        Serial.print(temp%10);
    }

    if (hum != INVALID_HUM)
    {
        Serial.print(" Hum:");
        Serial.print(hum);
        Serial.print('%');
    }
    if (pressure != INVALID_PRESSURE)
    {
        Serial.print(" Baro:");
        Serial.print(pressure);
    }
    if (PressureSeaLevel != INVALID_PRESSURE)
    {
        Serial.print(" BaroSea:");
        Serial.print(PressureSeaLevel);
    }
    if (power != INVALID_POWER)
    {
        Serial.print(" Power:");
        Serial.print(power);
        Serial.print(" Total Power:");
        Serial.print(totalpower);
    }
    if (Rain != INVALID_RAIN)
    {
        Serial.print(" Rain:");
        Serial.print(Rain);
    }

    printRSSI();
//     Serial.print(" RSSI:");Serial.print(radio.readRSSI());

    if (getReportType() == SERIAL_DEBUG) {
        if (data) {
            Serial.print(' ');
//            printBinary(data, pos, 8);
            printHexa(data, pos );
        }
    }
    Serial.println();
}

