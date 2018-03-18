set version=1.6.7
set avr=c:\arduino-%version%\hardware\tools\avr\bin
set file=Ook_OSV12.ino


%avr%\avr-objdump  -S   %file%.elf > %file%.lst

pause

