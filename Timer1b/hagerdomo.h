#define CMD_ECO    0 
#define CMD_CONFOR 1 
#define CMD_HGEL   2 
#define CMD_ARRET  3 
#define CMD_CONFIG 4 

extern void HagerSends(byte id4 , byte cmnd, byte NbTime) ;
void HagerSetPin (byte tx, byte pled);
void HagerSends1(byte id4, byte cmnd, byte NbTime);


