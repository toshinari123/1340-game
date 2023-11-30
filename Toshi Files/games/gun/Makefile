FLAGS = -pedantic-errors -std=c++11

main_gun_game.o: main_gun_game.cpp gun_game.h
	g++ $(FLAGS) -c $< -lncurses

main_gun_game: main_gun_game.o
	g++ $(FLAGS) $^ -o $@ -lncurses

clean:
	rm -f  *.o main_gun_game

.PHONY: clean
