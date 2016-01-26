
extern byte  Z1Eco [];
extern byte  Z1Conf[];
extern  byte  Z2Eco [] ;
extern  byte  Z2Conf[] ;
extern byte  Z3Eco [];
extern byte  Z3Conf[];
extern byte  ConfZ1[];
extern byte  ConfZ2[];
extern byte  ConfZ3[];
extern byte  Hz1   [];
extern byte  Hz2   [];
extern byte  Header[];

byte Addr2   [] = {0x07,0x04,0xF1,0x00,0x02 };
byte Addr3   [] = {0x07,0x04,0xF2,0x00,0x02 };

//send two time
void HagerSend(byte * Pulse )
{
  cli();
  HagerSendOne( Pulse );
  sei();
  delay(10);
  cli();
  HagerSendOne( Pulse );
  sei();
  delay(10);
}

void ManageHagerOld(byte id4 , byte cmnd)
{
  switch(id4)
  {
    //commande hager : subtype==0
    case 1 :
          if (cmnd==1)
            HagerSend(Z1Conf );
          else
            HagerSend(Z1Eco );
          break;  
    case 2 :
        if (cmnd==1)
          HagerSend(Z2Conf );
        else
          HagerSend(Z2Eco );
        break;  
    case 3 :
        if (cmnd==1)
          HagerSend(Z3Conf );
        else
          HagerSend(Z3Eco );
        break;  
    case 4 :
        HagerSend(Hz1 );
        break;  
    case 5 :
        HagerSend(Hz2 );
        break;  
    case 6 :
      HagerSend(Hz2 );
      break;  
    case 7 :
      HagerSend(ConfZ1 );
      break;  
    case 8 :
      HagerSend(ConfZ2 );
      break;  
    case 9 :
      HagerSend(ConfZ3 );
      break;  
    case 0 :
     
      HagerSend ( Addr2 , cmnd );
  		delay(10);
      HagerSend ( Addr2 , cmnd );
  		delay(10);
      
      break;  
    case 10 :
      HagerSend ( Addr1 , cmnd );
  		delay(10);
      HagerSend ( Addr1 , cmnd );
  		delay(10);
      break;        
    case 11 :
      HagerSend ( Addr3 , cmnd );
  		delay(10);
      HagerSend ( Addr3 , cmnd );
  		delay(10);
      
      break;  
  }
}

