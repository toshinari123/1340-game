game.o: game.cpp game.h
	g++ -c $<

main.o: main.cpp game.h game.cpp
	g++ -c $<

main: main.o game.o
	g++ $^ -o $@ -lncurses
