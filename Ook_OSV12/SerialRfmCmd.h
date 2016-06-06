    
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
       radio.writeReg(addr, val) ;
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
    }
    if (input == 'x')
    {
      detachInterrupt(1);
      easy.initPin();
      radio.setMode(RF69_MODE_TX);
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
        delay(3000);        // wait 3 sec
        easy.setSwitch(false,55,1);    // turn on device 0
        Serial.println("Sent");
      }
    if (input == 'M')
      {
        easy.setSwitch(true,55,1);    // turn on device 0
      }
    if (input == 'A')
      {
        easy.setSwitch(false,55,1);    // turn on device 0
      }
