#!/bin/bash

#cd Allfiles 
#make clean
#make maze
#chmod u+x maze
#toshi/out/game 2> err.txt

make clean
make game
chmod u+x out/game
out/game 2> err.txt
