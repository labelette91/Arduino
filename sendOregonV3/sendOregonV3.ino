
#include "sendOregon.h"

// #define THN132N
 
#define LED_PIN 9
 
 
// Buffer for Oregon message

#ifdef THN132N
  #define DATA_SIZE 8  
#else
  #define DATA_SIZE 9
#endif

sendOregon OregonMessageBuffer(3);


/******************************************************************/
/******************************************************************/
float Temp ; 
void setup()
{
  pinMode(LED_PIN, OUTPUT);
 
  Serial.begin(9600);
  Serial.println("\n[Oregon V2.1 encoder]");
 
 
#ifdef THN132N  
  // Create the Oregon message for a temperature only sensor (TNHN132N)
  byte ID[] = {0xEA,0x4C};
#else
  // Create the Oregon message for a temperature/humidity sensor (THGR2228N)
  byte ID[] = {0x1A,0x2D};
#endif  
 
  setType(OregonMessageBuffer.Data(), ID);
  setChannel(OregonMessageBuffer.Data(), 0x20);
  setId(OregonMessageBuffer.Data(), 0xBB);
  Temp=(float)11.2;
}

void PrintMessage(byte* data, byte dataSize )
{
  for (byte i = 0; i < dataSize ; ++i)   {     
    Serial.print(data[i] >> 4, HEX);
    Serial.print(data[i] & 0x0F, HEX);
  }
 Serial.println();
 
}

void reportSerial ( byte * data, byte pos ) {
// Energy OWL : CMR180
    if ( (data[0] == CMR180_ID0 ) && (data[1] == CMR180_ID1 ) )
    {
       Serial.print("[CMR180,...] Id:");
       Serial.print(data[0], HEX);Serial.print(data[1], HEX);
       Serial.print(", size:");
       Serial.print(pos);
       Serial.print(" ,power:");
       Serial.print(getPower(data)); 
       if (pos > 6) {
         // Display only for main frame
         // Secondary frame is only 6 Bytes long
         Serial.print(" ,total:");
         Serial.print(getTotalPower(data));
         Serial.print(" ,total kWh:");
         Serial.print(getTotalPower(data)/3600/1000);
       }
       Serial.println();
    }
}

void PulseLed(byte ledPin)
{
      if (digitalRead(ledPin)== LOW) 
         digitalWrite(ledPin, HIGH);  
      else
         digitalWrite(ledPin, LOW);  
}


void sendTemp()
{
  // Get Temperature, humidity and battery level from sensors
  // (ie: 1wire DS18B20 for temp�rature, ...)
  setBatteryLevel(OregonMessageBuffer.Data(), 0); // 0 : low, 1 : high
  setTemperature(OregonMessageBuffer.Data(), Temp);
  Temp += (float)0.1 ;
 
#ifndef THN132N
  // Set Humidity
  setHumidity(OregonMessageBuffer.Data(), 52);
#endif  
 
  // Calculate the checksum
  OregonMessageBuffer.CalculateAndSetChecksum(DATA_SIZE);
 
  // Show the Oregon Message
  PrintMessage(OregonMessageBuffer.Data(),DATA_SIZE);
 
  // Send the Message over RF
  // message two time 
  OregonMessageBuffer.Send( DATA_SIZE );
  
}

void loop()
{
  sendTemp();

/* setPowerPacketType(OregonMessageBuffer.Data(),1 );
 setTotalPower(OregonMessageBuffer.Data(),0x12345678 );
 setPower(OregonMessageBuffer.Data(),0x1234 );
OregonMessageBuffer.CalculateAndSetChecksum(10);

 PrintMessage(OregonMessageBuffer.Data(),10);
 OregonMessageBuffer.Send(10  );
*/  
  // Wait for 30 seconds before send a new message 
  delay(30000);
 PulseLed(LED_PIN);
  
}
