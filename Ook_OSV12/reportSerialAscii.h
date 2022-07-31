
char DectoHex(byte v)
{
    if (v<=9)
        return (v+'0');
    else
        return (v+'A'-10);
}

void printBinary ( byte * data , byte pos)
{
	for (byte i = 0; i < pos; ++i)
	{
		byte bt = data[i];
		for (byte b = 0; b < 8; b++)
		{
			if (bt & 0x80)
				Serial.print('1');
			else
				Serial.print('0');
			bt = bt << 1;
			if (b == 3) Serial.print(' ');
			if (b == 7) Serial.print(' ');

		}
	}
}

//sump hex
void printHexa (byte * data, byte pos)
{

for (byte i = 0; i < pos; ++i) {
	Serial.print(DectoHex( data[i] >> 4 ));
	Serial.print(DectoHex(data[i] & 0x0F));
}
}

void printTab(byte tab, byte n)
{
	tab = tab - n;
	while (tab>0 ) {
		Serial.print(' ');
		tab--;
	}

}

void reportSerialAscii (const char* s, const byte* data,byte pos) {
	  byte nb;
    Serial.print(s);
    Serial.print(' ');
    Serial.print(millis() / 1000);
/*    Serial.print(" Dt:");
		nb = Serial.print(Dt);
		printTab(2, nb);
		
		Serial.print(" Nb:");
		nb = Serial.print(NbReceive);
		printTab(3, nb);
*/	
		Serial.print(" Np:");
    nb=Serial.print(NbPulse);
		printTab(6, nb);
		Serial.print(' ');


//sump binaire
    printHexa (data, pos);
    
//    Serial.println();
 // Outside/Water Temp : THN132N,...
    if(data[0] == 0xEA && data[1] == 0x4C)
    {
       Serial.print(" [THN132N] Id:");
       Serial.print(data[3], HEX);
       Serial.print(" ,Channel:");
       Serial.print(channel(data));
       Serial.print(" ,temp:");
       Serial.print(temperature(data));
       Serial.print(" ,bat:");
       Serial.print(battery(data));
    }
    // Inside Temp-Hygro : THGR228N,...
    else if(data[0] == 0x1A && data[1] == 0x2D)
    {
       Serial.print("[THGR228N] Id:");
       Serial.print(data[3], HEX);
       Serial.print(" Ch:");
       Serial.print(channel(data));
       Serial.print(" T:");
       Serial.print(temperatureEnt(data));
       Serial.print(".");
       Serial.print(temperatureFrac(data));

       Serial.print(" ");
	   Serial.print(temperatureint(data));

	   Serial.print(" ");
	   Serial.print(getHumidity(data));
       Serial.print("% bat:");
       Serial.print(battery(data));
    }
		// Energy OWL : CMR180
    else if ( (data[0] == CMR180_ID0 ) && (data[1] == CMR180_ID1 ) )
    {
       Serial.print("[CMR180,...] Id:");
       Serial.print(data[0], HEX);Serial.print(data[1], HEX);
       Serial.print(", size:");
       Serial.print(pos);
       Serial.print(" ,power:");
       Serial.print(getPower((byte*)data)); 
       if (pos > 6) {
         // Display only for main frame
         // Secondary frame is only 6 Bytes long
         Serial.print(" ,total:");
         Serial.print(getTotalPower((byte*)data));
         Serial.print(" ,total kWh:");
         Serial.print(getTotalPower((byte*)data)/3600/1000);
       }
    }

#ifdef RFM69
     Serial.print(" RSSI:");Serial.print(radio.readRSSI());
#endif
     Serial.println();
    
}

