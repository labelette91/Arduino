class DeltaT {

unsigned long Last ;
    
public:
DeltaT() {
   Last=0; 
}
//return true if time since last time record is greater than deltatT
bool deltaT (word pDeltaT /* ms */){
  if ( ( millis() - Last ) > pDeltaT )
    return true ;
  else
    return false ;
}
void recordTime(){
   Last = millis(); 
}

};



class Record {
public:

#define LINELENGTH  64    

#define MAXREC 256    
word Pulse[MAXREC];
byte PulseNb  ;
long LastPrint ;
byte printed;

void reset(){
	PulseNb=0;
}
void init(){
//    for (byte i=0;i<MAXREC;i++)Pulse[i]=0;
    reset();
    LastPrint=0;
    printed=0;
}
//record pulse in a fifo maner 
void AddFifo(word p ){
      Pulse[PulseNb++]=p ;
}
//record pulse and stop when buffer is full
void Add(word p ){
    if (PulseNb<MAXREC)
      Pulse[PulseNb++]=p ;
}

void print4 (word p){
     if (p<10)  Serial.print("000");
else if (p<100) Serial.print("00");
else if (p<1000)Serial.print("0");
Serial.print(p);
};

void print2 (word p){
if (p<10)Serial.print("0");
Serial.print(p);
};

void print(word p){
 
  Serial.print(p);
  if ((++printed%LINELENGTH)==0)
    Serial.println(' ');
  else   
    Serial.print(' ');
}

void printDelta(word p , word deltaT ){
  word Dt = (millis() - LastPrint) ;
  if (Dt>deltaT){
      Serial.println();
      printed=0;
  }
  print(p);
  LastPrint = millis() ;    
}

//print last len records with fifo
void PrintFifo( byte len ){
  byte j;
      printed=0;
      for (byte i=0;i<len;i++){
      	j = PulseNb - len + i    	;
      	print(Pulse[j] )   	;
      }
      Serial.println(' ');    

      LastPrint = millis() ;

}

//print all record and reset
void Print(){
  PrintFifo(PulseNb);
  PulseNb=0;
  
}

};