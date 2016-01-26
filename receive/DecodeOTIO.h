class DecodeOTIO {
public:
    byte total_bits, state, MaxTemp , NbTempOk ;
    long T , Temp , tmpTemp, lastTemp;
    long LastReceived ; 
public:
    enum { UNKNOWN, T0, T1, T2, T3, OK, DONE };

    void resetDecoder () {
          total_bits =  0;
          state = UNKNOWN;
          T = 0 ;
    }    
    DecodeOTIO (byte pMaxTemp=5) { Temp = 0 ; NbTempOk=0;MaxTemp=pMaxTemp;tmpTemp=0;lastTemp=0;LastReceived=0;resetDecoder(); }

    bool nextOnePulse (word width , byte data)   {
       /* 1er bit */ 
       if ( (width>1900)&& (data==1) )   state = T0 ;
      /* bit next  */
      if ( (width>1000)&&(state == T0) )
      {
        total_bits++;
        /*  end of word and correct number of bit  */
        if ( width > 6000)
        {
          if (total_bits==25)
          {
            //update temperature
            tmpTemp = T ;
            resetDecoder ();
            return true;
          }
          else
          {
            resetDecoder ();
            return false;
          }
        } 
        else
        {
          /* add bit to temp */
          T = T << 1;
          T = T | getPulseBit(width);
        }
    }
    if (total_bits>=25)  resetDecoder ();
    return false;   
  }
  bool nextPulse (word width , byte data)   {
    if (nextOnePulse ( width ,  data) ){
       if (NbTempOk==0) lastTemp = tmpTemp ;
       if (lastTemp == tmpTemp ) NbTempOk ++ ; else NbTempOk -- ;
       if (NbTempOk >= MaxTemp ){
          NbTempOk = 0 ;
          Temp = lastTemp ;
          if ( (millis()-LastReceived) > 2000)
          {
            LastReceived = millis();
            return true;
          }
          else
            return false ;
            
       } 
    }
    return false ;

  }  
  

/*
NB --IDENT-------    SIGN- -----TEMP---    -BAT---   --FRAC-    End Temp
   3 2 1 0 9 8 7 6   5     4 3 2 1 0 9 8   7 6 5 4   3 2 1 0     
25 1 0 0 1 0 1 0 1   1     0 0 0 1 0 1 1   0 0 0 0   0 0 1 0    88  -11.2
25 1 0 0 1 0 1 0 1   1     0 0 0 0 0 0 0   0 0 0 0   0 0 1 0    87  -0.2                                                      
25 1 0 0 1 0 1 0 1   0     0 0 0 0 0 0 1   0 0 0 0   0 0 1 0    88  01.2                                                      
25 1 0 0 1 0 1 0 1   0     0 0 0 1 0 0 0   0 0 0 0   0 0 1 0    88  08.2
25 1 0 0 1 0 1 0 1   0     0 0 0 1 0 1 0   0 0 0 0   0 1 0 1    87  10.5
*/
  /* temperature in Lsb = 0.1 degrec C */
  int getTemperature(){
    int  t = Temp ;
    byte frac = t & 0xF ;
    t = t >>8 ;
    byte signe = t &  0x80 ;
    /* partie entiere */
    t = t & 0x7F;
    /* partie frac */
    t = t*10  ;
    t = t  + frac ;
    if (signe)
      t = -t;
    return t;        
    
  }
  byte getId(){
    return (Temp >> 16) ;        
  }
  byte getBatteryOk(){
    return ( (Temp &  0x80 )!= 0 ) ;
  }
  void ReportSerial(){
    Serial.print ("Otio:");
    Serial.print (Temp,HEX);
    Serial.print (" T:");
    int t = getTemperature();
    Serial.print (t/10);
    Serial.print ('.');
    t=t%10; if (t<0)t=-t;
    Serial.print (t);
    Serial.print (" Id:");
    Serial.print (getId(),HEX);
    Serial.print (" Bat:");
    Serial.println (getBatteryOk());

  }
};
