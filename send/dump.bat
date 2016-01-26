set avr=I:\Domo\arduino-1.0.5-r2\hardware\tools\avr\bin
%avr%\avr-objdump -d -S -t  %1.elf > %1.lst
%avr%\avr-size         %1.elf >>%1.lst
%avr%\avr-size         %1.hex >>%1.lst

grep -w ".text" %1.lst > %1.map
grep -w ".bss"  %1.lst >>%1.map
grep -w ".data" %1.lst >>%1.map

REM Example:
REM 
REM avr-size spacetrash.hex 
REM    text    data     bss     dec     hex filename
REM       0    7168       0    7168    1c00 spacetrash.hex
REM avr-size spacetrash.elf 
REM    text    data     bss     dec     hex filename
REM    7130      38     421    7589    1da5 spacetrash.elf
REM Flash-ROM size: 38 + 7130 = 7168 bytes
REM None-PROGMEM data (copyied from ROM to RAM): 38 bytes
REM PROGMEM data is placed in the text area and does not appear separately
REM Required RAM: 38 + 421 + stack = 459 bytes + stack
REM Text = code   data=ram initiazlise bss = ram 