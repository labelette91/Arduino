#pragma once

typedef enum {
REPORT_DOMOTIC,
REPORT_SERIAL       ,
SERIAL_DEBUG 
} T_REPORTTYPE;

void setReportType(T_REPORTTYPE pReportType );
T_REPORTTYPE getsetReportType();
bool isReportSerial();
        
void registerStdout() ;
char DectoHex(byte v);
void printBinary ( byte * data , byte pos, byte space  );
//sump hex
void printHexa ( byte * data, byte pos);
void printTab(byte tab, byte n);
