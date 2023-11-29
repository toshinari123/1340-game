#!/bin/bash

#make clean
make game
chmod u+x toshi/games/maze/game toshi/games/hangman/hangman toshi/games/gun/main toshi/games/blackjack/blackjack
out/game 2> err.txt
