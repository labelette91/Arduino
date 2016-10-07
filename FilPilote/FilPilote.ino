#include <EEPROM.h>


#include "DecodeHomeEasy.h"
DecodeHomeEasy HEasy(1) ;

#include "fifo.h"
TFifo  fifo;

#define ledPin  9
//pin for RF data input : ext_int_1
#define PDATA 3 

#define PIN1    4
#define PIN2    5
#define PIN3    6
#define PIN4    7
#define PIN5    8
#define PIN6    9
#define PIN7   10
#define PIN8   11

//learning input pin
#define PINLEARNING   12

//led status pin
#define PINSTATUS     13

long int 	LastReceive ;
long int 	LearningTime ;

word 		Dt;
byte        pinData;

byte        gid[4] ;
byte        learningInProgress;
byte        pinlearning ;


//le signal du RFM69 entre sur int ext    d3 : int1

#define VERSION 1
#define EMPTY_ADDR 0xff 
static unsigned long  last;
void ext_int_1(void) {
    
    word   pulse ;

    pinData=digitalRead(PDATA);

    // determine the pulse length in microseconds, for either polarity
    pulse = micros() - last;
    last += pulse;
    fifo.put(pulse);
}

void setStatusLed()
{
    if ( (gid[0]!=EMPTY_ADDR)&&(gid[1]!=EMPTY_ADDR)&&(gid[2]!=EMPTY_ADDR)&&(gid[3]!=EMPTY_ADDR) )
        digitalWrite(PINSTATUS,HIGH);
    else
        digitalWrite(PINSTATUS,LOW );

}
void setup () {
	  LastReceive = 0 ;
    last=0;
    
//    Serial.begin(38400);
    Serial.begin(115200);
    
   // initialize the LED pin as an output:
    pinMode(ledPin, OUTPUT);       
    pinMode(PDATA, INPUT_PULLUP);


/*    pinMode(PIN1, OUTPUT);       
    pinMode(PIN2, OUTPUT);       
    pinMode(PIN3, OUTPUT);       
    pinMode(PIN4, OUTPUT);       
    pinMode(PIN5, OUTPUT);       
    pinMode(PIN6, OUTPUT);       
    pinMode(PIN7, OUTPUT);       
    pinMode(PIN8, OUTPUT);       
*/
    pinMode(PINLEARNING, INPUT_PULLUP );       
    pinMode(PINSTATUS  , OUTPUT);       
   
    for (byte i=0;i<4;i++)gid[i]=0;

    for (byte i=0;i<4;i++)
        EEPROM.get(i,gid[i]);

    setStatusLed();

    learningInProgress=0;
    LearningTime=-30000;

    attachInterrupt(1, ext_int_1, CHANGE);

    HEasy.resetDecoder();
    Serial.print("Version ");
    Serial.println(VERSION);
    Serial.print( "Adress: " ) ; 
    for (byte i=0;i<4;i++) Serial.print(gid[i],HEX);Serial.println();
}

void PulseLed()
{
      if (digitalRead(ledPin)== LOW) 
         digitalWrite(ledPin, HIGH);  
      else
         digitalWrite(ledPin, LOW);  
}

void loop () {

byte id[4],unitcode,cmnd    ;


    word p = fifo.get();

/*     Dt = (millis() - LastReceive)/1000;
     if (Dt==5)
         digitalWrite(ledPin, LOW);  

    if (Dt==1){
    }    */
    if (p != 0) {
        if (HEasy.total_bits>1){
          Serial.print(HEasy.total_bits,HEX);Serial.println();
        }
        if (HEasy.nextPulse(p,pinData )) {

	        id[0]        = HEasy.data[5];            /* id emetteur 0..3  */
	        id[1]        = HEasy.data[6];            /* id emetteur 0..FF */
	        id[2]        = HEasy.data[7];            /* id emetteur 0..FF */
	        id[3]        = HEasy.data[8];            /* id emetteur 0..FF */
	        unitcode   = HEasy.data[2];     	 /* unit = zone 1..3  */
            cmnd       = HEasy.data[3] ;           /* cmd */
            Serial.print( "Receive : " ) ; for (byte i=2;i<9;i++) Serial.print(HEasy.data[i],HEX);Serial.println();
            //if no learning In Progress
            if (learningInProgress==0){
                if ( (id[0]==gid[0])&&(id[1]==gid[1])&&(id[2]==gid[2])&&(id[3]==gid[3]) ){
                    switch(unitcode){
                        case 1: digitalWrite(PIN1,cmnd) ;   break;
                        case 2: digitalWrite(PIN2,cmnd) ;   break;
                        case 3: digitalWrite(PIN3,cmnd) ;   break;
                        case 4: digitalWrite(PIN4,cmnd) ;   break;
                        case 5: digitalWrite(PIN5,cmnd) ;   break;
                        case 6: digitalWrite(PIN6,cmnd) ;   break;
                        case 7: digitalWrite(PIN7,cmnd) ;   break;
                        case 8: digitalWrite(PIN8,cmnd) ;   break;
                    }
                }
            }
            else{
                //clear  adress 
                for (byte i=0;i<4;i++)gid[i]=0;
                learningInProgress=0;
                for (byte i=0;i<4;i++)
                    EEPROM.put(i,gid[i]);

                Serial.print( "Learn :" ) ; for (byte i=5;i<9;i++) Serial.print(HEasy.data[i],HEX);Serial.println();
                setStatusLed();
            }

//        PulseLed();
          HEasy.resetDecoder();
      	}
    }
    //learning button pressed
    pinlearning = digitalRead(PINLEARNING) ;
    if ((learningInProgress==0)&&(pinlearning==0)){
        if ((millis()-LearningTime) >30000 ){
          LearningTime=millis();
          learningInProgress=1;
          Serial.println( "Learning in progress" ) ; 
        }
    }
    //Blink status led during learning
    if (learningInProgress==1){
        Dt = millis() ;
        Dt = Dt % 1000;
        if (Dt <500)
            digitalWrite(PINSTATUS,HIGH);
        else
            digitalWrite(PINSTATUS,LOW);

        //timeout 30 secondes
        if ((millis()-LearningTime) >30000 ){
            learningInProgress=0 ;
            Serial.println( "Learning timout" ) ; 
            setStatusLed();
            
        }

        //clear adress if learning pressed during 3 seconds 
        if ( (pinlearning==0) &&  ((millis()-LearningTime) >3000 )  ){
            gid[0]        =  EMPTY_ADDR ;            /* id emetteur 0..3  */
            gid[1]        =  EMPTY_ADDR ;            /* id emetteur 0..FF */
            gid[2]        =  EMPTY_ADDR ;            /* id emetteur 0..FF */
            gid[3]        =  EMPTY_ADDR ;             /* id emetteur 0..FF */
            learningInProgress=0;
            for (byte i=0;i<4;i++)
                EEPROM.put(i,gid[i]);
            Serial.println( "Learning reset" ) ; 
            setStatusLed();

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

