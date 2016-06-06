#include "arduino.h"
#include <avr/pgmspace.h>

//firt pulse = 1 length in dizaine de micro seconds 
const byte Header[] PROGMEM= {
21 ,	//0 : 1 / 208
22 ,	//1 : 0 / 212
21 ,	//2 : 1 / 208
22 ,	//3 : 0 / 212
21 ,	//4 : 1 / 208
22 ,	//5 : 0 / 212
21 ,	//6 : 1 / 208
22 ,	//7 : 0 / 212
21 ,	//8 : 1 / 208
22 ,	//9 : 0 / 212
21 ,	//10 : 1 / 208
22 ,	//11 : 0 / 212
21 ,	//12 : 1 / 208
22 ,	//13 : 0 / 212
21 ,	//14 : 1 / 208
22 ,	//15 : 0 / 212
21 ,	//16 : 1 / 208
22 ,	//17 : 0 / 212
21 ,	//18 : 1 / 208
22 ,	//19 : 0 / 212
21 ,	//20 : 1 / 208
22 ,	//21 : 0 / 212
21 ,	//22 : 1 / 208
22 ,	//23 : 0 / 212
21 ,	//24 : 1 / 208
21 ,	//25 : 0 / 208
22 ,	//26 : 1 / 212
21 ,	//27 : 0 / 208
22 ,	//28 : 1 / 212
21 ,	//29 : 0 / 208
22 ,	//30 : 1 / 212
21 ,	//31 : 0 / 204
63 ,	//32 : 1 / 624
63 ,	//33 : 0 / 624
21 ,	//34 : 1 / 208
22 ,	//35 : 0 / 212
63 ,	//36 : 1 / 624
63 ,	//37 : 0 / 624
21 ,	//38 : 1 / 208----21
22 ,	//39 : 0 / 212
0 ,	//40 : 1 / 208--21

};
