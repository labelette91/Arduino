#pragma once

#define REPORT_DOMOTIC  1  
#define REPORT_SERIAL   2  
#define SERIAL_DEBUG    4 

#define T_REPORTTYPE byte

#define INVALID_BYTE 0x7F
#define INVALID_TEMP  0x7FFF
#define INVALID_HUM   0x7F
#define INVALID_POWER  0x7FFF
#define INVALID_PRESSURE  0x7FFF
#define INVALID_RAIN      0x7FFF


#define TAB 10

void setReportType(T_REPORTTYPE pReportType );
T_REPORTTYPE getReportType();
bool isReportSerial();
bool isReportDomotic();

        
void registerStdout() ;
char DectoHex(byte v);
void printBinary ( byte * data , byte pos, byte space  );
//sump hex
void printHexa ( byte * data, byte pos);
void printTab(byte tab, byte n);

void reportPrintHeader();

void reportSerial(const char* Name, byte id1, byte id2, byte bateryLevel, int temp, byte hum, word power, unsigned long totalpower, word pressure,word PressureSeaLevel, word Rain , byte* data, byte pos);
