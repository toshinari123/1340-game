FLAGS = -pedantic-errors -std=c++11

gun_game.o: gun_game.cpp gun_game.h popup.h
	g++ $(FLAGS) -c $<
popup.o: popup.cpp popup.h game.h
	g++ $(FLAGS) -c $<
main_gun_game.o: main_gun_game.cpp gun_game.o
	g++ $(FLAGS) -c $<

main_gun_game: main_gun_game.o gun_game.o popup.o
	g++ $(FLAGS) $^ -o $@ -lncurses

clean:
	rm -f gun_game.o popup.o main_gun_game.o main_g

.PHONY: clean
