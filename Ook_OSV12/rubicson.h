
class DecodeRubicson : public DecodeOOK {
public:

    bool SyncReceived = 0 ;

public:
	enum { UNKNOWN, T0, T1, T2, T3, OK, DONE };

	//pMaxTemp : the number of successive received equal value to return the current temperature
	//OTIO send 20 frames with the 24 bits value */

	DecodeRubicson() { 
        resetDecoder(); 
        PacketCountSeuil=3;
        Name="R";
    }


#define TO(VALUE)(VALUE)

	// return -1 : error in decoder 
	// 1 : end of reception 
	// 0 : continue
	char decode(word pWidth, byte data) {

		word width = TO(pWidth);

		//pulse to high = 500us
		if (data == 1)
		{
			if ((width > TO(300)) && (width < TO(700)))
            {
                if ( SyncReceived ){
                    //last end sync in a packet begin
                    state = OK ;
                    SyncReceived=false;
                }
				return 0;
            }
			 else
				 return -1;
		}

		//pulse low
		if (data == 0)
		{

            if (state == OK )
            {
                //if end pulse : 9200us
			    if ((width > TO(7000)) && (width < TO(9800)))
			    {
                    //if end of frame : pulse = 9200us
                    if (total_bits == 36){ 
		                //rubicson
                        SyncReceived = true;
                        return 1;
                    }
				    else if (total_bits == 24) {
					    //otio
                        SyncReceived = true;
					    return 1;
				    }
				    else {
                        SyncReceived = true;
                        return -1;
                    }
                }

			    //if one  pulse : 3800us
			    if ((width > TO(3200)) && (width < TO(4400)))
			    {
				    gotBit(1);
				    return 0;
			    }

			    //if 0  pulse : 1900 us
			    if ((width > TO(1600)) && (width < TO(2200)))
			    {
					    gotBit(0);
					    return 0;
			    }
            }
            else
            {
                //synchro pulse
			    if ((width > TO(7000)) && (width < TO(9800))){
                    state = OK ;
                    return 0;
                }
            }
		}
        SyncReceived = false;
		return -1;
  }

	/*Message Format : (9 nibbles, 36 bits) :
		*
		* Format for Temperature Humidity
		    0         1         2         3        4    
		*   AAAA AAAA AAAA BCDD EEEE EEEE EEEE FFFF FFFF
		*
		*   A = Rolling Code
		*   B = Battery: 12
		*   C = 0 = scheduled transmission, 1 = requested transmission(button press)
		*   D = Channel number(00 = ch1 01 = ch2 10 = ch3)
		*   E = Temperature(two's complement)
			*   F = Humidity BCD format

0101 1111 0011 0000 0001 1001 0110 0101 0111

Each (group) of numbers has a specific meaning:

    Header 0 till 3
    ID: 4 till 11
    Battery: 12
    TX mode: 13
    Channel: 14 till 15
    Temperature: 16 till 27
    Humidity: 28 till 35

0101 11110011 0 0 00 000110010110 01010111
0101 1111 0011 1000 0000 1001 0110 0101 0111

    The ID is defined as a binary number
0         1         2         3         4
5    F    3    8    0    C    B    2    B
0101 1111 0011 1000 0000 1100 1011 0010 1011 0000

    The Battery identifies the state of the battery
    The TX mode defines whether the signal was sent automatic or manual
    The Channel is defined as a binary number and specifies which channel the sensor uses
    The Temperature is defined as a binary number and represents the temperature
    The Humidity is defined as a binary number and represents the humidity

    temp = 0000 1100 1011 : 0CB = 203
    hum  = 0010 1011      : 2B  = 43
*/

  /* temperature in Lsb = 0.1 degrec C */
  int getTemperatureRubi() {
	  int temp = data[2];
	  temp = (temp << 4) + ((data[3] & 0xF0) >> 4);
	  if (data[2] & 0x80)
		  temp = 4096 - temp;
	  return temp;

  }
  byte  gethumidityRubi() {
	  byte hum = (data[3] & 0x0F) << 4;
		   hum += (data[4] & 0xF0) >> 4;

	  return hum;

  }
  //return identification 0x00 to 0xff
  byte getIdRubi() {
	  byte id = (data[0] & 0x0F );
         id = id << 4 ;
		   id += (data[1] & 0xF0) >> 4;
	  return (id);
  }
  //return 15 if batterie OK  
  byte getBatteryLevelRubi() {
	  if (data[1] & 0x8)
		  return 15;
	  else
		  return 0;
  }
  byte getChannelRubi() {
	  return  (data[1] & 0x3)+1;
  }

//otio
/*
frame  0 = short pulse = 2ms
1 = long  pulse = 4ms
2 = end   pulse = 8ms
--IDENT-------    SIGN- -----TEMP---    -BAT---   --FRAC-    End Temp
2 2 2 2 1 1 1 1   1     1 1 1 1 1 0 0   0 0 0 0   0 0 0 0
              0                     1                   2   
7 6 5 4 3 2 1 0   7     6 5 4 3 2 1 0   7 6 5 4   3 2 1 0  
1 0 0 1 0 1 0 1   1     0 0 0 1 0 1 1   0 0 0 0   0 0 1 0    2  -11.2
1 0 0 1 0 1 0 1   1     0 0 0 0 0 0 0   0 0 0 0   0 0 1 0    2  -0.2
1 0 0 1 0 1 0 1   0     0 0 0 0 0 0 1   0 0 0 0   0 0 1 0    2  01.2
1 0 0 1 0 1 0 1   0     0 0 0 1 0 0 0   0 0 0 0   0 0 1 0    2  08.2
1 0 0 1 0 1 0 1   0     0 0 0 1 0 1 0   0 0 0 0   0 1 0 1    2  10.5
*/
/* temperature in Lsb = 0.1 degrec C */
	int getTemperatureOtio() {
		byte frac = data[2]  & 0xF;

		byte signe = data[1]  & 0x80;

		/* partie entiere */
		byte ent = data[1]  & 0x7F;

		int temp  = ent  * 10;
		temp = temp + frac;
		if (signe)
			temp = -temp;
		return temp ;

	}
	//return identification 0x00 to 0xff
	byte getIdOtio() {
		return (data[0] );
	}
	//return 15 if batterie OK  
	byte getBatteryLevelOtio() {
		if ((data[2] & 0x80) != 0)
			return 15;
		else
			return 0;
	}


	bool isOtio()
	{
		if (total_bits == 24)
			return true;
		else
			return false;
	}

    virtual void report()
    {
        if (isOtio())
        {
            if(isReportSerial())  printTab(TAB,Serial.print("OTIO "));
            reportDomoticTemp(getTemperatureOtio(), getIdOtio(), 0, getBatteryLevelOtio(),data,pos);
        }
        else
        {
            if(isReportSerial())  printTab(TAB,Serial.print("RUBI "));
            reportDomoticTempHum(getTemperatureRubi(), gethumidityRubi(), getIdRubi(), getChannelRubi(), getBatteryLevelRubi(), sTypeTH10_RUBiCSON,data,pos );
        }

    }

};


