#include "DecodeRain.h"
#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>            //assumes Arduino IDE v1.0 or greater
#else
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

#endif
#include "util.h"

/**
TFA Drop is a rain gauge with a tipping bucket mechanism.
Links:
 - Product page:
   - https://www.tfa-dostmann.de/en/produkt/wireless-rain-gauge-drop/
 - Manual 2019:
   - https://clientmedia.trade-server.net/1768_tfadost/media/2/66/16266.pdf
 - Manual 2020:
   - https://clientmedia.trade-server.net/1768_tfadost/media/3/04/16304.pdf
 - Discussion of protocol:
   - https://github.com/merbanan/rtl_433/issues/1240
The sensor has part number 30.3233.01. The full package, including the
base station, has part number 47.3005.01.
The device uses PWM encoding:
- 0 is encoded as 250 us pulse and a 500us gap
- 1 is encoded as 500 us pulse and a 250us gap
Note that this encoding scheme is inverted relative to the default
interpretation of short/long pulses in the PWM decoder in rtl_433.
The implementation below thus inverts the buffer. The protocol is
described below in the correct space, i.e. after the buffer has been
inverted.
Not every tip of the bucket triggers a message immediately. In some
cases, artificially tipping the bucket many times lead to the base
station ignoring the signal completely until the device was reset.
Data layout:
```
CCCCIIII IIIIIIII IIIIIIII BCUU XXXX RRRRRRRR CCCCCCCC SSSSSSSS MMMMMMMM
KKKK
```
- C: 4 bit message prefix, always 0x3
- I: 2.5 byte ID
- B: 1 bit, battery_low. 0 if battery OK, 1 if battery is low.
- C: 1 bit, device reset. Set to 1 briefly after battery insert.
- X: Transmission counter
     - Possible values: 0x0, 0x2, 0x4, 0x6, 0x8, 0xA, 0xE, 0xE.
     - Rolls over.
- R: LSB of 16-bit little endian rain counter
- S: MSB of 16-bit little endian rain counter
- C: Fixed to 0xaa
- M: Checksum.
   - Compute with reverse Galois LFSR with byte reflection, generator
     0x31 and key 0xf4.
- K: Unknown. Either b1011 or b0111.
     - Distribution: 50:50.
[Bitbench](http://triq.net/bitbench) string:
```
ID:hh ID:hh ID:hh BAT_LOW:b RESET:b UNKNOWN:bb XMIT_COUNTER:h RAIN_A:d
CONST:hh RAIN_B:d CHECK:8b UNKNOWN:bbxx xxxx
```
Some example data:
```
c240aaff09550021c
c240aabf095500e04
c240aafd095500b64
c240aafb0955003e4
c240aaf9095500a9c
c212b7f9035500e5c
c212b7f703550053c
c212b7f5035500c44
```
The rain bucket counter represents the number of tips of the rain
bucket. Each tip of the bucket corresponds to 0.254mm of rain.
The rain bucket counter does not start at 0. Instead, the counter
starts at 65526 to indicate 0 tips of the bucket. The counter rolls
over at 65535 to 0, which corresponds to 9 and 10 tips of the bucket.
If no change is detected, the sensor will continue broadcasting
identical values. This lasts at least for 20 minutes,
potentially forever.
The second nibble of byte 3 is a transmission counter: 0x0, 0x2, 0x4,
0x6, 0x8, 0xa, 0xc, 0xe. After the transmission with counter 0xe, the
counter rolls over to 0x0 on the next transmission and the cycle starts
over.
After battery insertion, the sensor will transmit 7 messages in rapid
succession, one message every 3 seconds. After the first message,
the remaining 6 messages have bit 1 of byte 3 set to 1. This could be
some sort of reset indicator.
For these 6 messages, the transmission counter does not increase.
After the full 7 messages, one regular message is sent after 30s.
Afterwards, messages are sent every 45s.
*/

uint16_t getRaw16bValue(uint8_t* data, uint8_t offset, uint8_t size);
uint8_t getRaw08bValue(uint8_t* data, uint8_t offset, uint8_t size);

    
    DecodeRain::DecodeRain (byte pMaxCode) { 
        resetDecoder(); 
        PacketCountSeuil=pMaxCode;
        Name ="R";
        max_bits = 17*4;

        SetPulseDuration(500,250,250,500,750,750);
    }
 
void reportDomoticRain( byte id1   ,byte id2   ,byte id3   ,byte id4   ,word rain  ,byte battery_low,  byte* data, byte pos);
/*
0        8        16       24        32       40       48       56 
CCCCIIII IIIIIIII IIIIIIII BCUU XXXX RRRRRRRR CCCCCCCC SSSSSSSS MMMMMMMM
KKKK
```
- C: 4 bit message prefix, always 0x3
- I: 2.5 byte ID
- B: 1 bit, battery_low. 0 if battery OK, 1 if battery is low.
- C: 1 bit, device reset. Set to 1 briefly after battery insert.
- X: Transmission counter
     - Possible values: 0x0, 0x2, 0x4, 0x6, 0x8, 0xA, 0xE, 0xE.
     - Rolls over.
- R: LSB of 16-bit little endian rain counter
- S: MSB of 16-bit little endian rain counter
- C: Fixed to 0xaa
- M: Checksum.
   - Compute with reverse Galois LFSR with byte reflection, generator
     0x31 and key 0xf4.
- K: Unknown. Either b1011 or b0111.
     - Distribution: 50:50.
  */
  void DecodeRain::report(){

        byte id1 = data[0];
        byte id2 = data[1];
        byte id3 = data[2];
        byte id4 = 0;
        byte rainlsb = data[4] ;
        word rain    = data[6] ;
        rain <<=8;
        rain += rainlsb;

        byte battery_low   = getRaw08bValue((uint8_t*)data,24,1);

      reportDomoticRain(  id1   , id2   , id3   , id4   , rain  , battery_low, data , pos );
  }

  bool DecodeRain::isValid()      		 
  {	  
      /*
     * Validate checksum
     */
    uint8_t checksum = data[7];
    uint8_t computed_checksum = lfsr_digest8_reflect(data, 7, 0x31, 0xf4);
    if (checksum != computed_checksum) 
        return false ;
    else
        return true  ;

  }    


