#include <RFM69.h>
#include <SPI.h>
#include <HomeEasyTransmitter.h>


#define NODEID      1
#define NETWORKID   100
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define LED         9
#define SERIAL_BAUD 38400

#define PDATA 3 //pin for data input/output
#define PCLK  4 //pin for clk  input/output

RFM69 radio;

// This is the Home Easy controller
// This example will use a 433AM transmitter on
// pin 3 : data pin
// pin 4 : clk  pin
HomeEasyTransmitter easy(PDATA,PCLK,LED);


word Pulse[256];

byte          Pin ;
byte          lastPin ;
unsigned long LastPulseTime ;
unsigned long pulse ;
byte 					NbPulse  ;

static unsigned long  last;

void ext_int_1(void) {
    // determine the pulse length in microseconds, for either polarity
    pulse = micros() - last;
//    last += pulse;
    last = micros() ;
    if (NbPulse<255){
      Pulse[NbPulse++]=pulse;
    }
}


void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(10);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  char buff[50];
  sprintf(buff, "\nListening at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);
  pinMode(PDATA, INPUT);
  pinMode(LED, OUTPUT);
  Pin     = 0 ;
  lastPin = 0 ;
  LastPulseTime = 0 ;
  pulse = 0 ;
  NbPulse = 0  ;
  last=0;
 

}

#define HEXTODEC(AH) AH = AH-'0'; if (AH>9) AH = AH -( 7 );if (AH>15) AH = AH - 0x20 ;



void print4 (long p){
if (p<10)Serial.print("000");
else if (p<100)Serial.print("00");
else if (p<1000)Serial.print("0");
Serial.print(p);
Serial.print(' ');
};

void print (long p){
if (p<10)Serial.print("0");
Serial.print(p);
Serial.print(' ');
};

void loop() {


  //process any serial input
  if (Serial.available() > 0)
  {
    char input = Serial.read();
    if (input == 'p') { //print pulse 
      Serial.print(NbPulse);
      for (byte i=0;i<NbPulse;i++){
        if (i%32==0)
          Serial.println();
      	print(Pulse[i]/100 )   	;
      }
      	NbPulse=0;
      	Serial.println();
    }
    
    
    
    if (input == 'd') //d=dump all register values
      radio.readAllRegs();

    if (input == 'r') // RXX read register number xx 
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
    if (input == 'w') // WAAVVwrite register number xx 
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
       
       
       Serial.print(" Write Adr:");Serial.print(addr,HEX);
       Serial.print("=");       Serial.println(val,HEX);
       byte regVal = radio.readReg(addr);
       radio.PrintReg( addr , regVal);    
    }
    

    if (input == 'i')
    {
      Serial.print(" [RX_RSSI:");Serial.print(radio.readRSSI());Serial.println("]");
    }

    if (input == 'a')
    {
      attachInterrupt(1, ext_int_1, CHANGE);
      pinMode(PDATA, INPUT);
      radio.setMode(RF69_MODE_RX);
       Serial.println("Receive mode");
    }
    if (input == 'x')
    {
      detachInterrupt(1);
      easy.initPin();
      radio.setMode(RF69_MODE_TX);
       Serial.println("Transmit mode");
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
    if (input == 's')
      {
        easy.setSwitch(true,55,1);    // turn on device 0
//        easy.setSwitch(true,55,1);    // turn on device 0
//        easy.setSwitch(true,55,1);    // turn on device 0
        delay(3000);        // wait 3 sec
        easy.setSwitch(false,55,1);    // turn on device 0
        Serial.println("Sent");
      }
    if (input == 'M')
      {
        easy.setSwitch(true,55,1);    // turn on device 0
        Serial.println("On");

      }
    if (input == 'A')
      {
        easy.setSwitch(false,55,1);    // turn on device 0
        Serial.println("Off");
      }

  }
	
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
