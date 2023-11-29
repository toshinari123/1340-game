FLAGS = -std=c++11 -pthread -g

OTHERS = out/main.o out/Main_Menu_Daemon.o out/Select_Game_Menu_Daemon.o out/misc.o

GAMES = games/blackjack/blackjack games/gun/main games/hangman/hangman game/maze/game

game: $(OTHERS) $(GAMES)
	g++ $(FLAGS) $(OTHERS) -o out/game -lncursesw

out/misc.o: generics/misc.cpp generics/misc.h
	g++ $(FLAGS) -o $@ -c $< -lncursesw

out/main.o: main.cpp
	g++ $(FLAGS) -o $@ -c $< -lncursesw

out/Main_Menu_Daemon.o: daemons/Main_Menu_Daemon.cpp daemons/Main_Menu_Daemon.h
	g++ $(FLAGS) -o $@ -c $< -lncursesw

out/Select_Game_Menu_Daemon.o: daemons/Select_Game_Menu_Daemon.cpp daemons/Select_Game_Menu_Daemon.h
	g++ $(FLAGS) -o $@ -c $< -lncursesw

games/blackjack/blackjack: games/blackjack/blackjack.cpp
	cd ./games/blackjack; g++ $(FLAGS) blackjack.cpp -o blackjack -lncurses; cd ../..

games/gun/main: games/gun/game.cpp games/gun/main.cpp games/gun/popup.cpp
	cd ./games/gun; make main; cd ../..

games/hangman/hangman: games/hangman/hangman.cpp games/hangman/hangman.h games/hangman/main.cpp
	cd ./games/hangman; make hangman; cd ../..

game/maze/game: games/maze/maze.cpp
	cd ./games/maze; make game; cd ../..

clean:
	rm out/*; cd ./games/blackjack; rm -f blackjack.o; cd ../gun; make clean; cd ../hangman; make clean; cd ../maze; make clean; cd ../..

.PHONY: clean
