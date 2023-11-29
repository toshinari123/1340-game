FLAGS = -pedantic-errors -std=c++11

hangman_main.o: hangman_main.cpp hangman.h
	g++ $(FLAGS) -c $<

hangman.o: hangman.cpp hangman.h
	g++ $(FLAGS) -c $<

hangman: hangman.o hangman_main.o
	g++ $(FLAGS) $^ -lncurses -o $@

gun_game.o: gun_game.cpp gun_game.h popup.h
	g++ $(FLAGS) -c $<

popup.o: popup.cpp popup.h gun_game.h
	g++ $(FLAGS) -c $<

main_gun_game.o: main_gun_game.cpp gun_game.o
	g++ $(FLAGS) -c $<


main_gun_game: main_gun_game.o gun_game.o popup.o
	g++ $(FLAGS) $^ -o $@ -lncurses

maze: maze.cpp
	g++ $(FLAGS) $^ -lncurses -o $@

clean:
	rm -f gun_game.o popup.o main_gun_game.o main_gun_game hangman.o hangman_main.0 hangman maze

.PHONY: clean