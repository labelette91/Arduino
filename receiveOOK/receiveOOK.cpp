#include <RFM69.h>
#include <SPI.h>
//#include <SPIFlash.h>

#define NODEID      1
#define NETWORKID   100
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define LED         9
#define SERIAL_BAUD 115200

#define PDATA 2 //pin for data input/output
#define PCLK  3 //pin for clk  input/output

RFM69 radio;

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(10);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  char buff[50];
  sprintf(buff, "\nListening at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);

  pinMode(PDATA, INPUT);
  pinMode(LRD , OUTPUT);

}

#define HEXTODEC(AH) AH = AH-'0'; if (AH>9) AH = AH -( 7 );


byte ackCount=0;
void loop() {
  //process any serial input
  if (Serial.available() > 0)
  {
    char input = Serial.read();
    if (input == 'r') //d=dump all register values
      radio.readAllRegs();
    
    if (input == 'R') // RXX read register number xx 
    {
       while (Serial.available() ==0 );
       char Ah  = Serial.read();
       while (Serial.available() ==0 );
       char Al  = Serial.read();
       HEXTODEC(Ah);
       HEXTODEC(Al);
       byte addr = Ah*16+ Al ; 
       Serial.print(" Read :");Serial.println(addr,HEX);
       byte regVal = radio.readReg(addr);
       radio.PrintReg( addr , regVal);    
    }
    if (input == 'W') // WAAVVwrite register number xx 
    {
       while (Serial.available() ==0 );
       char Ah  = Serial.read();
       while (Serial.available() ==0 );
       char Al  = Serial.read();
       HEXTODEC(Ah);
       HEXTODEC(Al);
       byte addr = Ah*16+ Al ; 

       while (Serial.available() ==0 );
       char Vh  = Serial.read();
       while (Serial.available() ==0 );
       char Vl  = Serial.read();
       HEXTODEC(Vh);
       HEXTODEC(Vl);
       byte val  = Vh*16+ Vl ; 
       
       
       Serial.print(" Write :");Serial.print(addr,HEX);
       Serial.print(":");       Serial.println(val,HEX);
       radio.writeReg(addr, val) ;
    }
    

    if (input == 's')
    {
      Serial.print(" [RX_RSSI:");Serial.print(radio.readRSSI());Serial.println("]");
    }

    if (input == 'a')
    {
      radio.setMode(RF69_MODE_RX);
    }
    if (input == 't')
    {
      byte temperature =  radio.readTemperature(-1); // -1 = user cal factor, adjust for correct ambient
      byte fTemp = 1.8 * temperature + 32; // 9/5=1.8
      Serial.print( "Radio Temp is ");
      Serial.print(temperature);
      Serial.print("C, ");
      Serial.print(fTemp); //converting to F loses some resolution, obvious when C is on edge between 2 values (ie 26C=78F, 27C=80F)
      Serial.println('F');
    }
  }

  if (digitalRead(PDATA)==1)
    digitalWrite(LED,HIGH);
  else
     digitalWrite(LED,LOW);
	
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
