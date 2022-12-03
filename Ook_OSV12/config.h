//type de report serie 
//si = define  : report serial forma domoticz (binaire)
//#define REPORT_TYPE  REPORT_DOMOTIC

//si =  : report serial format text 
//#define REPORT_TYPE REPORT_SERIAL 
#define REPORT_TYPE SERIAL_DEBUG 

#ifndef WIN32
//#define RFM69_ENABLE
//#define  BMP180_ENABLE        
#endif 

//#define OTIO_ENABLE        1
  #define OOK_ENABLE         2
//#define HAGER_ENABLE       3
#define HOMEEASY_ENABLE    4
//#define MD230_ENABLE       5
  #define RUBICSON_ENABLE    6
  #define HIDEKI_ENABLE      7
  #define RAIN_ENABLE        8

// #define RASPBERRY_PI

//offset in micros for pulse duration for RFM69 : 80Micros
#define OFFSET_DURATION_HIGH 0
