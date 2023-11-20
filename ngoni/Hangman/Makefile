FLAGS = -pedantic-errors -std=c++11

main.o: main.cpp hangman.h
	g++ $(FLAGS) -c $<

hangman.o: hangman.cpp hangman.h
	g++ $(FLAGS) -c $<

hangman: hangman.o main.o
	g++ $(FLAGS) $^ -lncurses -o $@
