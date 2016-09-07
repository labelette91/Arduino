#include "arduino.h"
#include <avr/pgmspace.h>
//firt pulse = 1 length in dizaine de micro seconds 
const byte Z3Eco[] PROGMEM = {
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
21 ,	//15 : 0 / 208
22 ,	//16 : 1 / 212
21 ,	//17 : 0 / 208
22 ,	//18 : 1 / 212
21 ,	//19 : 0 / 208
22 ,	//20 : 1 / 212
21 ,	//21 : 0 / 208
21 ,	//22 : 1 / 208
22 ,	//23 : 0 / 212
21 ,	//24 : 1 / 208
22 ,	//25 : 0 / 212
21 ,	//26 : 1 / 208
22 ,	//27 : 0 / 212
21 ,	//28 : 1 / 208
22 ,	//29 : 0 / 212
21 ,	//30 : 1 / 208
21 ,	//31 : 0 / 208
63 ,	//32 : 1 / 624
63 ,	//33 : 0 / 624
21 ,	//34 : 1 / 208
21 ,	//35 : 0 / 208
63 ,	//36 : 1 / 624
63 ,	//37 : 0 / 628
21 ,	//38 : 1 / 208
21 ,	//39 : 0 / 208
22 ,	//40 : 1 / 211
21 ,	//41 : 0 / 204
22 ,	//42 : 1 / 211
21 ,	//43 : 0 / 204
21 ,	//44 : 1 / 208
21 ,	//45 : 0 / 208
21 ,	//46 : 1 / 208
21 ,	//47 : 0 / 208
21 ,	//48 : 1 / 208
42 ,	//49 : 0 / 416
21 ,	//50 : 1 / 208
21 ,	//51 : 0 / 208
21 ,	//52 : 1 / 203
22 ,	//53 : 0 / 212
42 ,	//54 : 1 / 411
21 ,	//55 : 0 / 208
21 ,	//56 : 1 / 208
21 ,	//57 : 0 / 208
21 ,	//58 : 1 / 208
21 ,	//59 : 0 / 208
21 ,	//60 : 1 / 208
21 ,	//61 : 0 / 208
21 ,	//62 : 1 / 208
42 ,	//63 : 0 / 416
42 ,	//64 : 1 / 416
21 ,	//65 : 0 / 203
21 ,	//66 : 1 / 208
42 ,	//67 : 0 / 416
21 ,	//68 : 1 / 208
21 ,	//69 : 0 / 208
21 ,	//70 : 1 / 208
21 ,	//71 : 0 / 208
21 ,	//72 : 1 / 208
21 ,	//73 : 0 / 208
21 ,	//74 : 1 / 203
22 ,	//75 : 0 / 212
42 ,	//76 : 1 / 416
42 ,	//77 : 0 / 411
42 ,	//78 : 1 / 416
21 ,	//79 : 0 / 208
21 ,	//80 : 1 / 208
21 ,	//81 : 0 / 208
21 ,	//82 : 1 / 208
21 ,	//83 : 0 / 208
21 ,	//84 : 1 / 208
21 ,	//85 : 0 / 204
22 ,	//86 : 1 / 212
21 ,	//87 : 0 / 203
21 ,	//88 : 1 / 208
21 ,	//89 : 0 / 208
21 ,	//90 : 1 / 208
21 ,	//91 : 0 / 208
21 ,	//92 : 1 / 208
21 ,	//93 : 0 / 208
21 ,	//94 : 1 / 208
21 ,	//95 : 0 / 208
21 ,	//96 : 1 / 208
21 ,	//97 : 0 / 208
21 ,	//98 : 1 / 208
21 ,	//99 : 0 / 203
22 ,	//100 : 1 / 212
21 ,	//101 : 0 / 204
21 ,	//102 : 1 / 208
21 ,	//103 : 0 / 208
21 ,	//104 : 1 / 208
21 ,	//105 : 0 / 208
21 ,	//106 : 1 / 208
42 ,	//107 : 0 / 416
42 ,	//108 : 1 / 416
21 ,	//109 : 0 / 203
22 ,	//110 : 1 / 212
21 ,	//111 : 0 / 203
21 ,	//112 : 1 / 208
42 ,	//113 : 0 / 416
42 ,	//114 : 1 / 416
21 ,	//115 : 0 / 208
21 ,	//116 : 1 / 208
21 ,	//117 : 0 / 208
21 ,	//118 : 1 / 208
21 ,	//119 : 0 / 208
21 ,	//120 : 1 / 208
21 ,	//121 : 0 / 203
22 ,	//122 : 1 / 212
42 ,	//123 : 0 / 411
42 ,	//124 : 1 / 416
42 ,	//125 : 0 / 416
42 ,	//126 : 1 / 416
21 ,	//127 : 0 / 208
21 ,	//128 : 1 / 208
21 ,	//129 : 0 / 208
21 ,	//130 : 1 / 208
42 ,	//131 : 0 / 416
21 ,	//132 : 1 / 203
21 ,	//133 : 0 / 208
42 ,	//134 : 1 / 416
21 ,	//135 : 0 / 208
21 ,	//136 : 1 / 208
42 ,	//137 : 0 / 416
21 ,	//138 : 1 / 208
21 ,	//139 : 0 / 208
42 ,	//140 : 1 / 416
42 ,	//141 : 0 / 416
21 ,	//142 : 1 / 203
21 ,	//143 : 0 / 208
42 ,	//144 : 1 / 416
0  ,	//145 : 0 / 0
0  ,	//146 : 1 / 0
0  ,	//147 : 0 / 0
0  ,	//148 : 1 / 0
0  ,	//149 : 0 / 0
0  ,	//150 : 1 / 0
0  ,	//151 : 0 / 0
};

