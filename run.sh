#!/bin/bash

cd Allfiles 
make clean
make maze
chmod u+x maze
toshi/out/game 2> err.txt
