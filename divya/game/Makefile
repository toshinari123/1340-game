game.o: game.cpp game.h popup.h
	g++ -c $<
popup.o: popup.cpp popup.h game.h
	g++ -c $<
main.o: main.cpp game.o
	g++ -c $<

main: main.o game.o popup.o
	g++ $^ -o $@ -lncurses
