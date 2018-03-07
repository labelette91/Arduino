class DecodeOOKV2 {
public:
	byte total_bits, bits, state, pos, data[25];

	virtual char decode(word width, byte level) { return -1; };

public:

	enum { UNKNOWN, T0, T1, T2, T3, OK, DONE };

	DecodeOOKV2() { resetDecoder(); }

	bool nextPulse(word width, byte level) {
		if (state != DONE)

			switch (decode(width, level)) {
			case -1: resetDecoder(); break;
			case 1:  done(); break;
			}
		return isDone();
	}

	bool isDone() const { return state == DONE; }

	const byte* getData(byte& count) const {
		count = pos;
		return data;
	}
	const byte* getData() const {
		return data;
	}

	void resetDecoder() {
		total_bits = bits = pos = 0;
		state = UNKNOWN;
	}

	// add one bit to the packet data buffer

	virtual void gotBit(char value) {
		total_bits++;
		byte *ptr = data + pos;
		*ptr = (*ptr << 1) | (value);

		if (++bits >= 8) {
			bits = 0;
			if (++pos >= sizeof data) {
				resetDecoder();
				return;
			}
		}
		state = OK;
	}


	void done() {
		while (bits)
			gotBit(0); // padding
		state = DONE;
	}
};


class DecodeRubicson : public DecodeOOKV2 {
public:
	long LastReceived;
public:
	enum { UNKNOWN, T0, T1, T2, T3, OK, DONE };

	//pMaxTemp : the number of successive received equal value to return the current temperature
	//OTIO send 20 frames with the 24 bits value */

	DecodeRubicson() { LastReceived = 0; resetDecoder(); }


#define TO(VALUE)(VALUE)

	// return -1 : error in decoder 
	// 1 : end of reception 
	// 0 : continue
	char decode(word pWidth, byte data) {

		word width = TO(pWidth);

		//pulse to long
		if ((width>TO(550)) && (data == 1))   return -1;

		//pulse low
		if (data == 0)
		{
      if (width > TO(5000)) {
        //if end of frame : pulse = 9000us
        if (total_bits == 36){ 
          LastReceived = millis();         
          return 1;
        }
        else
          return -1;
      }
        

			if (width > TO(3000))
			{
				gotBit(1);
			}
			else
			{
				if ( (width > TO(1500))  && (width < TO(2100)) )
					gotBit(0);
				else
					return -1;
			}
		}
		return 0;
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
  int getTemperature() {
	  int temp = data[2];
	  temp = (temp << 4) + ((data[3] & 0xF0) >> 4);
	  if (data[2] & 0x80)
		  temp = 4096 - temp;
	  return temp;

  }
  byte  gethumidity() {
	  byte hum = (data[3] & 0x0F) << 4;
		   hum += (data[4] & 0xF0) >> 4;

	  return hum;

  }
  //return identification 0x00 to 0xff
  byte getId() {
	  byte id = (data[0] & 0x0F );
         id = id << 4 ;
		   id += (data[1] & 0xF0) >> 4;
	  return (id);
  }
  //return 15 if batterie OK  
  byte getBatteryLevel() {
	  if (data[1] & 0x8)
		  return 15;
	  else
		  return 0;
  }
  byte getChannel() {
	  return  (data[1] & 0x3)+1;
  }

  void ReportSerial() {
	  Serial.print("Rubicson:");
	  Serial.print(" T:");
	  int t = getTemperature();
	  Serial.print(t / 10);
	  Serial.print('.');
	  t = t % 10; if (t < 0)t = -t;
	  Serial.print(t);
	  Serial.print(" Id:");
	  Serial.print(getId(), HEX);
	  Serial.print(" Chn:");
	  Serial.print(getChannel());
	  Serial.print(" Bat:");
	  Serial.print(getBatteryLevel());
	  Serial.print(" Hum:");
	  Serial.print(gethumidity());
	  Serial.print('\n');
	  Serial.print('\r');

  }
};






