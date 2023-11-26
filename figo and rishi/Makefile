FLAGS = -pedantic-errors -std=c++11

game: rishigame.cpp
	g++ $(FLAGS) $^ -lncurses -o $@

blackjack: blackjack.cpp
	g++ $(FLAGS) $^ -o $@

clean:
	rm -f game blackjack

.PHONY: clean
