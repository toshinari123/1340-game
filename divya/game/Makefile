FLAGS = -pedantic-errors -std=c++11

game.o: game.cpp game.h popup.h
	g++ $(FLAGS) -c $<
popup.o: popup.cpp popup.h game.h
	g++ $(FLAGS) -c $<
game.o: game.cpp gun_game.o
	g++ $(FLAGS) -c $<

game: game.o game.o popup.o
	g++ $(FLAGS) $^ -o $@ -lncurses

clean:
	rm -f gun_game.o popup.o main_gun_game.o main_g

.PHONY: clean
