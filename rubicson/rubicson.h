class DecodeRubicson : public DecodeOOK {
public:
	byte  MaxTemp, NbTempOk;
	long T, Temp, tmpTemp, lastTemp;
	long LastReceived;
public:
	enum { UNKNOWN, T0, T1, T2, T3, OK, DONE };

	void resetDecoder() {
		total_bits = 0;
		state = UNKNOWN;
		T = 0;
	}
	//pMaxTemp : the number of successive received equal value to return the current temperature
	//OTIO send 20 frames with the 24 bits value */

	DecodeRubicson(byte pMaxTemp = 5) { Temp = 0; NbTempOk = 0; MaxTemp = pMaxTemp; tmpTemp = 0; lastTemp = 0; LastReceived = 0; resetDecoder(); }

	/* p pulse time in 100 micros unit : return */
	/* 0 = short pulse = 2ms */
	/* 1 = long  pulse = 4ms */
	/* 2 = end   pulse = 8ms */
	byte getPulseBit(byte p)
	{
		if (p < 25)
			return 0;
		else
			if (p < 65)
				return 1;
			else
				return 2;
	}

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
			if (total_bits == 36)
			{
				//end
				return 1;
			}
		}
		return 0;
  }

	/*Message Format : (9 nibbles, 36 bits) :
		*
		* Format for Temperature Humidity
		    0         1         2         3        4    
		*   AAAA AAAA AAAA BCDD EEEE EEEE EEEE FFFF FFFF
		*   0101 1100 0001 1000 1111 0111 1011 0000 1110
		    0111 0000 0000 1111 1011 0000 0000 0000 0101
		    1011 0101 0000 1x00                0100 1001

		    0100 0101 1000 0110 1111 0000 1100 00100110
		    0101 1111 1101 1000 0000 1111 0001 00001110
		    0100 0101 1000 0010
		*
		*   A = Rolling Code
		*   B = 1 (fixed value)
		*   C = 0 = scheduled transmission, 1 = requested transmission(button press)
		*   D = Channel number(00 = ch1 01 = ch2 10 = ch3)
		*   E = Temperature(two's complement)
			*   F = Humidity BCD format
*/

  /* temperature in Lsb = 0.1 degrec C */
  int getTemperature() {
	  int temp = data[2];
	  temp = (temp << 4) + ((data[3] & 0xF0) >> 4);
	  if (data[3] & 0x80)
		  temp = 4096 - temp;
	  return temp;

  }
  byte  gethumidity() {
	  byte hum = (data[3] & 0x0F) << 4;
		   hum += (data[4] & 0xF0) >> 4;

	  return hum;

  }
  //return identification 0x00 to 0xff
  word getId() {
	  word id = (data[0] << 4);
		   id += (data[1] & 0xF0) >> 4;
	  return (id);
  }
  //return 15 if batterie OK  
  byte getBatteryLevel() {
	  if (data[1] & 0x80)
		  return 15;
	  else
		  return 0;
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
	  Serial.print(" Bat:");
	  Serial.print(getBatteryLevel());
	  Serial.print(" Hum:");
	  Serial.print(gethumidity());
	  Serial.print('\n');
	  Serial.print('\r');

  }
};






