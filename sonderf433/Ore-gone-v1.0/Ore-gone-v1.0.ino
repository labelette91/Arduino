
/* Radio Hub using a 433.92 MHz module 

   ** Temperature + Hygro using a Si7021 (also compatible with SHTX) I2C sensor
   ** low battery
   ** Power saving mode with idle wake up
   
   The system can use the Oregon Scientific protocol 2.1 and 3.0
   
   In version 2.1, the synch nibble 0xA is included in the type ID of the sensor (ex THGR228 : 1A2D)
   In version 3, the synch nibble is send on its own and the payload starts just after, so the sync nibble is combined
   with a payload nibble (byte [0])

   version 1.0 : 20-05-2016 - initial commit
   version 1.01 : 25-05-2016 - added switch use to force sending the temperature frame + easy pairing & couple of general fixes
   
*/


#include <Wire.h>
#include <SHT2x.h>
#include <LowPower.h>
#include <EEPROM.h>

#include "oregon.h"

byte LED_PIN = 3;
byte POWER_ENABLE = 2;
byte SWITCH = 4;
byte BATTERY = A3;

Sensor TempHygroSensor;

byte i;

// Storage
int Hygro;
float Temperature;
bool tempError = false;
int BatteryVoltage = 0;

// LOW BATTERY SETTINGS
// Voltage divider = 1:2. With a 2 x li-ion cell supply solution, low power is 6v. 
// (Battery PCB will cut @about 2.75 x 2 = 5.5V)
// Thresh = [(Thresh voltage / 2) / ADC Vref ] x 1023
// With a 5V/16 MHz pro mini arduino
// Thres = (6/2) / 5 * 1023 = 613.8
// To be noted that the low battery relies also on the brown out value of the MCU (burnt with the bootloader)
// and the selected regulator.
// When using only a pair of AA cells (probably not recommended), a 2.5V regulator should be used in place of the 3.3V.
// If no regulator is used, low battery might be impossible to detect without an additional voltage ref (ie zener or else)
// 
// In the case of rechargeable Ni-MH (1.2V nominal cells), thresholds below will have to be adapted as well.
// The HW doesn't have any protection against reverse polarity. A protection diode (regular or shottky) can be added or not
// depending on the (low) voltage of the supply solution (will affect performance due to voltage drop)

// Uncomment only one of the above to select the battery / power supply solution
//#define ONE_LI_ION_CELL
#define TWO_LI_ION_CELLS
//#define THREE_AAA
//#define FOUR_AAA

#ifdef ONE_LI_ION_CELL    // 3.3V reg
#define BATT_THRESH     511
#endif
#ifdef TWO_LI_ION_CELLS   // 5V reg
#define BATT_THRESH     614
#endif
#ifdef THREE_AAA            // 3.3V reg
#define BATT_THRESH     511
#endif
#ifdef FOUR_AAA            // 3.3V reg
#define BATT_THRESH     511
#endif


int BatteryThreshold = BATT_THRESH;
int BatteryThresholdHyst = 10;
bool LowBattery = false;

#define EEPROM_FORMATTED_TOKEN  0xAA
#define EEPROM_TOKEN            0x00
#define EEPROM_OREGON_ID        0x01
#define EEPROM_OREGON_CHANNEL   0x02

#define MAX_SERIAL  25
#define REFRESH_INTERVAL  30  // *8sec = 180 seconds = 3min
#define REFRESH_RANDOM    6
int RefreshInterval = REFRESH_INTERVAL; 
int RefreshCounter = 0;

const char Usage1[] PROGMEM  = {"help<CR> - Print this help"};
const char Usage2[] PROGMEM  = {"newid<CR> - Generate & store a new (random) Oregon ID"};
const char Usage3[] PROGMEM  = {"setid=<ID><CR> - Set and store a new Oregon ID"};
const char Usage4[] PROGMEM  = {"newch=<CH><CR> - Set and store a new channel [1;8]"};
const char Usage5[] PROGMEM  = {"defaults<CR> - Restore default ID and channel"};
const char Usage6[] PROGMEM  = {"quit<CR> - Quit the service menu"}; 

const char* const UsageTable[] PROGMEM = {Usage1, Usage2, Usage3, Usage4, Usage5, Usage6};

 
/******************************************************************/
 
void setup()
{
  pinMode(TX_PIN, OUTPUT);  // RF output pin to the 433 MHz module
  pinMode(LED_PIN, OUTPUT);
  pinMode(POWER_ENABLE, OUTPUT);
  pinMode(SWITCH, INPUT_PULLUP);

  // Serial Port at only 9600 bauds to accomodate slower arduinos (8MHz)
  Serial.begin(9600);
  digitalWrite(LED_PIN, HIGH);
  Serial.println("\nOre-Gone v1.0");
  Serial.println("Wireless 433.92 MHz Low Power Temperature & Humidity Sensor");

  // Set RF data pin to idle state
  SEND_LOW();
  
  // Pre computes all CRC8
  initCrc8();
  Wire.begin();

  // Enables the Battery Voltage reading and the RF Transmitter
  digitalWrite(POWER_ENABLE, HIGH);
  BatteryVoltage = analogRead(BATTERY);
  digitalWrite(POWER_ENABLE, LOW);
  Serial.println("Looking for Temperature sensor...");
  Temperature = SHT2x.GetTemperature();
  // Detect if the sensor is properly attached by pinging it. Do not further boot if sensor is damaged or not wired.
  if((Temperature < -40.) || (Temperature > 100.0))
  {
    Serial.print("Attempt to read Temp. Failed - ");
    Serial.println(Temperature);
    Serial.println("Sensor defective or unplugged - Stopping there...");
    while(1)
    {
      digitalWrite(LED_PIN, HIGH);
      delay(500);
      digitalWrite(LED_PIN, LOW);
      delay(500);
    } 
  }

  Serial.println("Sensor found");
  Serial.print("Current Temperature is ");
  Serial.print(Temperature);
  Serial.println(" degC");
  // Sensor seems ok, read the RH
  Hygro = SHT2x.GetHumidity();
  
  // Creates a random seed based on the environment
  int seed = BatteryVoltage + (int)Temperature + (int)Hygro;
   
  // Uses the battery voltage to initialize the random number generator
  randomSeed(seed);

  // Loads up the sensor params from EEPROM
  byte eeprom_value;
  byte UniqueID;
  byte Channel;
  eeprom_value = EEPROM.read(EEPROM_TOKEN);
  if(eeprom_value != EEPROM_FORMATTED_TOKEN)
  {
    Serial.println("Formatting EEPROM...");
    EEPROM.write(EEPROM_TOKEN, EEPROM_FORMATTED_TOKEN);
    UniqueID = CreateNewID();
    EEPROM.write(UniqueID, EEPROM_OREGON_ID);
  }
  else
  {
    UniqueID = EEPROM.read(EEPROM_OREGON_ID);
    Channel = EEPROM.read(EEPROM_OREGON_CHANNEL);
    if(Channel > 8)
    {
      Serial.println("Error, Channel must be {1;8}");
      Serial.println("Setting channel to 1");  
      Channel = 1;
      EEPROM.write(EEPROM_OREGON_CHANNEL, Channel);
    }
  }

  Serial.println("Loaded up Sensor params: ");
  Serial.print("ID = 0x");
  Serial.println(UniqueID,HEX);
  Serial.print("Channel = ");
  Serial.println(Channel);

  // Handle the service menu, available at boot only, before the sleep / power saving
  // system is installed (plus it's safer to not enable it during normal use)
  if(!digitalRead(SWITCH))
  {
    int QuitLoop = false;
    long ElapsedTime = millis();
    char SerialBuffer[MAX_SERIAL];
    char StringBuffer[MAX_SERIAL];
    byte SerialIndex = 0;
    byte TheChar;
    byte FlagSerial = false;

    Serial.println("Entering Service Menu");
    Serial.println("help<CR> for usage");
    
    while(!QuitLoop)
    {
      if (Serial.available())
      {
        while(Serial.available() && (SerialIndex < MAX_SERIAL))
        {
          TheChar = Serial.read();
          if((TheChar == '\n') || (TheChar == '\r'))
          {
            SerialBuffer[SerialIndex] = '\0';
            SerialIndex = 0;
            strcpy(StringBuffer, SerialBuffer);
            FlagSerial = true; 
          }
          else
          {
            SerialBuffer[SerialIndex++] = TheChar;
          }
        }
      }
      if(SerialIndex >= MAX_SERIAL)
        SerialIndex = 0;

      if(FlagSerial)
      {
       byte Index = 0;
       FlagSerial = false;
       if(!strncmp(StringBuffer, "newid", 5))
       {
        UniqueID = CreateNewID(); // randomize ID
        EEPROM.write(EEPROM_OREGON_ID,UniqueID);
       }
       else if(!strncmp(StringBuffer, "setid", 5))
       {
        Index = SkipToValue(StringBuffer);
        UniqueID = (byte)atoi(&StringBuffer[Index]);
        
        Serial.print("Setting ID to ");
        Serial.print(UniqueID);
        Serial.print(" - 0x");
        Serial.println(UniqueID,HEX);
        EEPROM.write(EEPROM_OREGON_ID,UniqueID);
      }
       else if(!strncmp(StringBuffer, "newch", 5))
       {
        Index = SkipToValue(StringBuffer);
        Channel = atoi(&StringBuffer[Index]);
        if(Channel > 8)
        {
          Serial.println("Error, Channel must be {1;8}");
          Serial.println("(RE)Setting channel to 1");  
          Channel = 1;
        }
        Serial.print("Setting channel to ");
        Serial.println(Channel);
        EEPROM.write(EEPROM_OREGON_CHANNEL, Channel);
      }
      else if(!strncmp(StringBuffer, "defaults", 5))
      {
        Serial.println("Restoring defaults");
        Channel = 1;
        UniqueID = 0xEE;
        EEPROM.write(EEPROM_TOKEN, EEPROM_FORMATTED_TOKEN);
        EEPROM.write(EEPROM_OREGON_ID,UniqueID);
        EEPROM.write(EEPROM_OREGON_CHANNEL, Channel);
      }
      else if(!strncmp(StringBuffer, "quit", 4))
      {
        Serial.println("Existing service menu");
        QuitLoop = true;
      }
      else if(!strncmp(StringBuffer, "help", 4))
      {
        PrintUsage(); 
      }
      else
      {
        Serial.print("Syntax error, unknown command : \"");
        Serial.print(StringBuffer);
        Serial.println("\"");
      }
     } // End of IF(FLAG_SERIAL)
    } // End of WHILE(!QUIT_LOOP)
  } // End of WHILE(!SWITCH)
  
  // Init sensor structure
  TempHygroSensor.Type.i = TEMP_HYGRO_OREGON;
  TempHygroSensor.Channel = Channel;
  TempHygroSensor.UniqueID = UniqueID;
  TempHygroSensor.MessageSize = 9;  // 9th byte is checksum
  TempHygroSensor.BytesToChecksum = 8;
  TempHygroSensor.LowBattery = false;

  PrepareSensor(&TempHygroSensor, OREGONV2);

  Serial.println("Going asleep in 2s");
  delay(2000);
  digitalWrite(LED_PIN, LOW);
}


void loop()
{ 

  // Forces the transmission of the temperature / oregon frame every times we pass here, useful when pairing
  // the probe, avoids waiting 4 minutes to get the device discovered in Domoticz.
  if(!digitalRead(SWITCH))
  {
    Serial.println("Force pairing");
    RefreshCounter = RefreshInterval;
    delay(10);
  }
  
  RefreshCounter++;  
  if(RefreshCounter > RefreshInterval)
  {
    RefreshCounter = 0;
    // Recomputes the refresh interval with a bit of randomicity to avoid any lock-synch
    RefreshInterval = REFRESH_INTERVAL + random(REFRESH_RANDOM);   // Random on the update time to avoid locking / jamming the transmission
    Serial.print("\nNext broadcast in ");
    Serial.print(RefreshInterval * 8);
    Serial.println(" Seconds");
    
    Temperature = SHT2x.GetTemperature();
    if(Temperature > 100. || Temperature < -50.)
    {
       tempError = true;
       Serial.println("Error Reading Temperature");
    }
    else
    {
      tempError = false;
      Hygro = (int)(SHT2x.GetHumidity());
      Serial.print("Temperature : ");
      Serial.print (Temperature);
      Serial.println(" C");
      Serial.print("Humidity : ");
      Serial.print(Hygro);
      Serial.println("%");
      setTemperature(&TempHygroSensor, Temperature);
     }
    
     digitalWrite(POWER_ENABLE, HIGH);
     delay(10);
     BatteryVoltage = analogRead(BATTERY);

     // Small state machine for storing the low battery bit with hysteresis
     if((BatteryVoltage < BatteryThreshold) && !LowBattery)
        LowBattery = true;
        
     if((BatteryVoltage > (BatteryThreshold + BatteryThresholdHyst)) && LowBattery)
        LowBattery = false;

     TempHygroSensor.LowBattery = LowBattery;

     Serial.print("Battery=");
     Serial.println(BatteryVoltage);
     Serial.print("LowBattery=");
     Serial.println(LowBattery);
     
     digitalWrite(LED_PIN, HIGH);
     if(!tempError)
     {
      setTemperature(&TempHygroSensor, Temperature);
      setHumidity(&TempHygroSensor, Hygro);
 
      // Calculate and store the checksum
      TempHygroSensor.MessageBuffer[8] = calculateAndSetChecksum(TempHygroSensor);
 
      // Send the Temp/Hygro Message over RF
      sendOregon(TempHygroSensor.MessageBuffer, TempHygroSensor.MessageSize, OREGONV2);
      // Send a "pause"
      SEND_LOW();
      delayMicroseconds(TWOTIME*8);
      // Send a copy of the first message. The v2.1 protocol send the
      // message twice
      sendOregon(TempHygroSensor.MessageBuffer, TempHygroSensor.MessageSize, OREGONV2);
      SEND_LOW();
    }
    
    // Power save
    digitalWrite(POWER_ENABLE, LOW);  
    digitalWrite(LED_PIN, LOW);
  }
  
  // ATmega328P, ATmega168
  LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);

  // 15 ms period for testing
  //LowPower.idle(SLEEP_15MS, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
}



byte CreateNewID(void)
{
  byte newID;
  newID = (byte)(random(255));
  Serial.print("New ID = 0x");
  Serial.println(newID, HEX);
  return newID;
}


void PrintUsage(void)
{
 char LocalString[60];
 int i;
 int len = sizeof(UsageTable) / sizeof(const char*);
 for(i = 0 ; i <  len ; i++)
 {
    strcpy_P(LocalString, (char*)pgm_read_word(&(UsageTable[i])));
    Serial.println(LocalString);
 }
}



//////////////////////////////////////////////////////////////////
// Look for the '=' sign then tries to find a value
unsigned char SkipToValue(char *StringBuffer)
{
  unsigned char i = 0;
  while(StringBuffer[i] != '=')
  {
    if(i >= MAX_SERIAL)
    {
      printf("Syntax error - '=' sign is missing\n");
      //printf("%s\n",StringBuffer);
      return(0);
    }
    i++;
  }
  i++;
  return(i);
}



