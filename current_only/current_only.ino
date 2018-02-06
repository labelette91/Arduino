// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3

#include "EmonLib.h"                   // Include Emon Library
#include "Types.h" 
#include <EEPROM.h>

#include "sendOregon.h"

sendOregon OregonMessageBuffer(3);
//temps d'envoi en seconde
#define DELTA_SEND 60

EnergyMonitor emon1;                   // Create an instance
//total power en Watt/h
//curent power en Watt/s
double TotalPower ;  
double Power  ;  
int min;
const double Tension = 230.0;

void PrintMessage(byte* data, byte dataSize )
{
  for (byte i = 0; i < dataSize ; ++i)   {     
    Serial.print(data[i] >> 4, HEX);
    Serial.print(data[i] & 0x0F, HEX);
  }
 Serial.println();
 
}
void WriteTotalPowerInEEP(double totalP  )
{
}
double  ReadTotalPowerInEEP( )
{
    
  return 0  ;
}

void PrintIrms ()
{
  Serial.print(emon1.Irms);         // Apparent power
  Serial.print(" ");
  Serial.print(Power);         // Apparent power
  Serial.print(" ");
  Serial.print  (TotalPower); 
  Serial.println("Wh");
}

void ManageSerial()
{
  if (Serial.available()){
        byte b = Serial.read();
        if (b=='c')
        {
          EEPROM.put(0,(double)0.0);
          Serial.println("Clear Total Power"); 
          TotalPower=0;
        }
        if (b=='d')
        {
          if (DEBUG==0) DEBUG=2;else  DEBUG=0;
          if (DEBUG)
            Serial.println("Debug all On "); 
           else
            Serial.println("Debug all Off "); 
           
        }
        if (b=='D')
        {
          if (DEBUG==0) DEBUG=1;else  DEBUG=0;
          if (DEBUG)
            Serial.println("Debug min On "); 
           else
            Serial.println("Debug min Off "); 
           
        }
        
  }      
}

void setup()
{  
  Serial.begin(115200);
  Serial.println("version 1.2" );  
  
  emon1.current(1, 111.1/2);             // Current: input pin, calibration.
  emon1.calibrate( 2000 ) ;							//calcul OffsetI
  Serial.print("OfsI:");Serial.println(emon1.offsetI);  

  //ClearTotalPowerInEEP();

  //restaure last Total power from eeprom
  EEPROM.get(0,TotalPower);

  Serial.print("TotalPower:"); Serial.println(TotalPower); 

 emon1.calcI (50*10,1000);  
  Serial.print("IZero ");
  Serial.println(emon1.Irms);         // Apparent power

  min = 0 ;
}
void loop()
{
  
/*  double Irms = emon1.calcIrms(1480*2);  // Calculate Irms only
  
  Serial.print(Irms*230.0);	       // Apparent power
  Serial.print(" ");
  Serial.print(Irms);		       // Irms
*/
	
  Power=0;
  for (int i=0;i<DELTA_SEND;i++)
  {
//    emon1.calcI (50,1000);  // Calculate Irms on 1 sec : 50 hz
    emon1.calcIrmsDuring(1000); //1seconde
    Power += emon1.Irms*Tension ;
    ManageSerial();
  }
  //en watt heure
  TotalPower+=(Power/3600) ;
  //power instantaner moyene sur 1 min
  Power /= DELTA_SEND;
  if (DEBUG>=1)
    PrintIrms ();
  emon1.calcIrms(5000); 
  min++;
  //sauvegarde toutes les heures
  if (min>=60)
  {
    min = 0 ;
  	EEPROM.put(0,TotalPower);
  }

  //send
   setPowerPacketType(OregonMessageBuffer.Data(),1 );
 
   setTotalPower(OregonMessageBuffer.Data(),0x12345678 );
   setPower(OregonMessageBuffer.Data(),0x1234 );
   setTotalPower(OregonMessageBuffer.Data(),  (long)(TotalPower*223.666 ) );//domoticz divise par 223.666 : unite Watt Heure
   setPower(OregonMessageBuffer.Data(),(int)Power );

   OregonMessageBuffer.CalculateAndSetChecksum(10);
//   PrintMessage(OregonMessageBuffer.Data(),10);
   OregonMessageBuffer.Send(10  );

}
