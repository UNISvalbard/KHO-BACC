@echo off 
mode com40 baud=9600 data=8 parity=n stop=1 dtr=off rts=off
echo 600 > \\.\COM40