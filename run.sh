#!/bin/bash

cd toshi
make clean
make game
chmod u+x games/maze/game games/hangman/hangman games/gun/main games/blackjack/blackjack
out/game 2> err.txt
