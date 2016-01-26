
volatile byte TurnDetected;
volatile boolean up;
byte LastClk ;
byte LastPinSw;
byte ms;


const int PinCLK=2;                   // Used for generating interrupts using CLK signal
const int PinDT=3;                    // Used for reading DT signal
const int PinSW=4;                    // Used for the push button switch

void isr ()  {                    // Interrupt service routine is executed when a HIGH to LOW transition is detected on CLK
 if (digitalRead(PinCLK))
   up = digitalRead(PinDT);
 else
   up = !digitalRead(PinDT);
 TurnDetected = true;
}


void setup ()  {
 pinMode(PinCLK,INPUT);
 pinMode(PinDT,INPUT);  
 pinMode(PinSW,INPUT);
// attachInterrupt (0,isr,FALLING);   // interrupt 0 is always connected to pin 2 on Arduino UNO
 Serial.begin (115200);
 Serial.println("Start");
 LastPinSw=1;
}

void loop ()  {
  word  time;
 static int virtualPosition=0;    // without STATIC it does not count correctly!!!
 static long LastDt;
 
 byte clk = digitalRead(PinCLK);
 byte PinSw = digitalRead(PinSW);
 if ((PinSw==0)&&(LastPinSw==1)) {      // check if pushbutton is pressed
//   virtualPosition=0;              // if YES, then reset counter to ZERO
//   Serial.print ("Reset = ");      // Using the word RESET instead of COUNT here to find out a buggy encoder
   
//   Serial.println (virtualPosition);
 }  
   LastPinSw = PinSw;
 
 if ((clk==0) && (LastClk==1))
 {
   up = digitalRead(PinDT);
   if (up) up=0;else up=1;
   TurnDetected = 1;
    time = millis()-LastDt ;
   LastDt = millis(); 
   if (time<500)  TurnDetected = 2;
   if (time<125)  TurnDetected = 10;
 }
 LastClk=clk;
 
 if (TurnDetected)  {		    // do this only if rotation was detected
   if (up)
     virtualPosition+=TurnDetected;
   else
     virtualPosition-=TurnDetected;
   if (virtualPosition>360)virtualPosition=0;
   if (virtualPosition<0)virtualPosition=360;
//   Serial.print ("Count = ");  
   Serial.print (virtualPosition);
   Serial.print ("  ");  
   Serial.println (time);
   TurnDetected = 0;          // do NOT repeat IF loop until new rotation detected
 }
 delay(4);
}

void loop1 ()  {
 static long virtualPosition=0;    // without STATIC it does not count correctly!!!

 if (!(digitalRead(PinSW))) {      // check if pushbutton is pressed
   virtualPosition=0;              // if YES, then reset counter to ZERO
   Serial.print ("Reset = ");      // Using the word RESET instead of COUNT here to find out a buggy encoder
   
   Serial.println (virtualPosition);
 }  
 
 if (TurnDetected)  {		    // do this only if rotation was detected
   if (up)
     virtualPosition+=TurnDetected;
   else
     virtualPosition-=TurnDetected;
   Serial.print ("Count = ");  
   Serial.println (virtualPosition);
   TurnDetected = false;          // do NOT repeat IF loop until new rotation detected
 }
}
