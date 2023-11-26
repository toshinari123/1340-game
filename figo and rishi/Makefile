FLAGS = -pedantic-errors -std=c++11

game: rishigame.cpp
	g++ $(FLAGS) $^ -lncurses -o $@

clean:
	rm -f game

.PHONY: clean
