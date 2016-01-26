
#ifndef HomeEasyTransmitter_h
#define HomeEasyTransmitter_h

class HomeEasyTransmitter
{

  public:
    //Constructor.
    //arg1 = pin to connected to 434Mhz transmitter
    HomeEasyTransmitter(short);
    //arg1 = true to turn switch on, false for off
    //arg2 = the 26 bit long transmitter ID as found by the Pete and Banarby's recevier code
    //arg3 = the 4 bit recipient code, seems to be on a dial for mains switches, 1010 (6 in decimal) for my light fittings
    void setSwitch(bool,unsigned long, short);
  private:
    short txPin;  // 433mhz transmitter on pin2

    void transmit(bool, unsigned long, short);
    void sendBit(bool);
    void sendPair(bool);

};

#endif


