/*
Code	Taille des trames
0x0A4D	80
0xEA4C	68
0xCA48	??
0x1A2D	80
0xFA28	80
0x*ACC	80
0xCA2C	80
0xFAB8	80
0x1A3D	80
0x5A5D	88
0x5A6D	96
0x2A1D	84
0x2A19	92
0x1A99	88
0x1A89	??
0x3A0D	80
0xEA7C	120
0xDA78	72
0x*AEC	104
0xEAC0	??


Sensor name	  Code	Type
Oregon-THR128	0x0A4D	Inside Temperature
Oregon-THR138		
Oregon-THC138		
Oregon-THC238	0xEA4C	Outside/Water Temp
Oregon-THC268		
Oregon-THN132N		
Oregon-THWR288A		
Oregon-THRN122N		
Oregon-THN122N		
Oregon-AW129		
Oregon-AW131		
Oregon-THWR800	0xCA48	Water Temp
Oregon-THGN122N	0x1A2D	Inside Temp-Hygro
Oregon-THGN123N		
Oregon-THGR122NX		
Oregon-THGR228N		
Oregon-THGR238		
Oregon-THGR268		
Oregon-THGR810	0xFA28	Inside Temp-Hygro
Oregon-RTGR328N	0x*ACC	Outside Temp-Hygro
Oregon-THGR328N	0xCA2C	Outside Temp-Hygro
Oregon-WTGR800	0xFAB8	Outside Temp-Hygro
Oregon-THGR918	0x1A3D	Outside Temp-Hygro
Oregon-THGRN228NX		
Oregon-THGN500		
Huger – BTHR918	0x5A5D	Inside Temp-Hygro-Baro
Oregon-BTHR918N	0x5A6D	Inside Temp-Hygro-Baro
Oregon-BTHR968		
Oregon-RGR126	0x2A1D	Rain Gauge
Oregon-RGR682		
Oregon-RGR918		
Oregon-PCR800	0x2A19	Rain Gauge
Oregon-WTGR800	0x1A99	Anemometer
Oregon-WGR800	0x1A89	Anemometer
Huger-STR918	0x3A0D	Anemometer
Oregon-WGR918		
Oregon-UVN128	0xEA7C	UV sensor
Oregon-UV138		
Oregon-UVN800	0xDA78	UV sensor
Oregon-RTGR328N	0x*AEC	Date & Time
cent-a-meter	0xEAC0	Ampere meter
OWL CM113		
Electrisave		
OWL CM119	0x1A**	Power meter
	0x2A**	
	0x3A**	
		

*/
#include "DecodeOOK.h"
// 433 MHz decoders


class OregonDecoderV2 : public DecodeOOK {
public:
    OregonDecoderV2() {}
    
    // add one bit to the packet data buffer
    virtual void gotBit (char value) {
        if(!(total_bits & 0x01))
        {
            data[pos] = (data[pos] >> 1) | (value ? 0x80 : 00);
        }
        total_bits++;
        pos = total_bits >> 4;
        /*compute size of paxket */
        if(pos == 2)
        {
          // Taille de trame par défaut (utilisée dans la majorité des sondes)
          max_bits = 160;
 
          // Exceptions :
          if(data[0] == 0x3A  && data[1] == 0x80)
            max_bits = (11*16); // CML180
			
          else  if(data[0] == 0xEA && data[1] == 0x4C)  // Outside/Water Temp : THN132N,...
            max_bits = 160;
          else if(data[0] == 0x1A && data[1] == 0x2D)   // Inside Temp-Hygro : THGR228N,...
            max_bits = 160;
          else
          {
            resetDecoder();
            return;
          }

/*
          else if(data[0] == 0xEA)
          {
            if(data[1] == 0x4C) max_bits = 136; // TH132N : 68 * 2
            else if(data[1] == 0x7C) max_bits = 240; // UV138 : 120 * 2
          }
          else if(data[0] == 0x5A)
          {
            if(data[1] == 0x5D) max_bits = 176; // THGR918 : 88 * 2
            else if(data[1] == 0x6D)max_bits = 192; // BTHR918N : 96 * 2
          }
          else if(data[0] == 0x1A  && data[1] == 0x99)
            max_bits = 176; // WTGR800 : 88 * 2
          else if(data[0] == 0xDA  && data[1] == 0x78)
            max_bits = 144; // UVN800 : 72 * 2
          else if((data[0] == 0x8A || data[0] == 0x9A) && data[1] == 0xEC)
            max_bits = 208; // RTGR328N 104 * 2
          else if(data[0] == 0x2A)
          {  
            if(data[1] == 0x19) max_bits = 184; // RCR800 : 92 * 2
            else if(data[1] == 0x1d) max_bits = 168; // WGR918 : 84 * 2
          }
*/          
        }
         
        /* */
        if (pos >= sizeof data) {
            resetDecoder();
            return;
        }
        state = OK;
    }
    
    virtual char decode (word width) {
//        if (200 <= width && width < 1200) {
        if (200 <= width && width < 1699) {
            byte w = width >= 700;
            switch (state) {
                case UNKNOWN:
                    if (w != 0) {
                        // Long pulse
                        ++flip;
//                    } else if (32 <= flip) {
                    } else if (w == 0 && 24 <= flip) {
                        // Short pulse, start bit
                        flip = 0;
                        state = T0;
                    } else {
                      // Reset decoder
                        return -1;
                    }
                    break;
                case OK:
                    if (w == 0) {
                        // Short pulse
                        state = T0;
                    } else {
                        // Long pulse
                        manchester(1);
                    }
                    break;
                case T0:
                    if (w == 0) {
                      // Second short pulse
                        manchester(0);
                    } else {
                        // Reset decoder
                        return -1;
                    }
                    break;
            }
        } else if (width >= 2500  && total_bits == max_bits ) {
            return 1;
        } else {
            return -1;
        }
        return total_bits == max_bits ? 1: 0;
    }
};

/*
class OregonDecoderV3 : public DecodeOOK {
public:
    OregonDecoderV3() {}
    
    // add one bit to the packet data buffer
    virtual void gotBit (char value) {
        data[pos] = (data[pos] >> 1) | (value ? 0x80 : 00);
        total_bits++;
        pos = total_bits >> 3;
        if (pos >= sizeof data) {
            resetDecoder();
            return;
        }
        state = OK;
    }
    
    virtual char decode (word width) {
        if (200 <= width && width < 1200) {
            byte w = width >= 700;
            switch (state) {
                case UNKNOWN:
                    if (w == 0)
                        ++flip;
                    else if (32 <= flip) {
                        flip = 1;
                        manchester(1);
                    } else
                        return -1;
                    break;
                case OK:
                    if (w == 0)
                        state = T0;
                    else
                        manchester(1);
                    break;
                case T0:
                    if (w == 0)
                        manchester(0);
                    else
                        return -1;
                    break;
            }
        } else {
            return -1;
        }
        return  total_bits == 80 ? 1: 0;
    }
};

*/

