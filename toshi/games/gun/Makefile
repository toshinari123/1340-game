FLAGS = -pedantic-errors -std=c++11

game.o: game.cpp game.h popup.h
	g++ $(FLAGS) -c $<
popup.o: popup.cpp popup.h game.h
	g++ $(FLAGS) -c $<
main.o: main.cpp game.o
	g++ $(FLAGS) -c $<

main: main.o game.o popup.o
	g++ $(FLAGS) $^ -o $@ -lncurses

clean:
	rm -f game.o popup.o main.o main

.PHONY: clean
