FLAGS = -pedantic-errors -std=c++11

menu: menu.cpp hangman main_gun_game blackjack maze
	g++ -pthread -std=c++11 menu.cpp -o menu -lncursesw

hangman_main.o: hangman_main.cpp hangman.h
	g++ $(FLAGS) -c $<

hangman.o: hangman.cpp hangman.h
	g++ $(FLAGS) -c $<

hangman: hangman.o hangman_main.o
	g++ $(FLAGS) $^ -lncurses -o $@

main_gun_game.o: main_gun_game.cpp gun_game.h
	g++ $(FLAGS) -c $< -lncurses

main_gun_game: main_gun_game.o
	g++ $(FLAGS) $^ -o $@ -lncurses

blackjack: blackjack_nc.cpp
	g++ $(FLAGS) $^ -lncurses -o $@

maze: maze.cpp
	g++ $(FLAGS) $^ -lncurses -o $@

clean:
	rm -f  *.o main_gun_game hangman maze blackjack menu

.PHONY: clean
