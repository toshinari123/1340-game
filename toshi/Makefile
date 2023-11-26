FLAGS = -std=c++11 -g

game: main.o Main_Menu_Daemon.o Select_Game_Menu_Daemon.o misc.o
	g++ $(FLAGS) $^ -o $@ -lncursesw

misc.o: generics/misc.cpp generics/misc.h
	g++ $(FLAGS) -c $< -lncursesw

main.o: main.cpp
	g++ $(FLAGS) -c $< -lncursesw

Main_Menu_Daemon.o: daemons/Main_Menu_Daemon.cpp daemons/Main_Menu_Daemon.h
	g++ $(FLAGS) -c $< -lncursesw

Select_Game_Menu_Daemon.o: daemons/Select_Game_Menu_Daemon.cpp daemons/Select_Game_Menu_Daemon.h
	g++ $(FLAGS) -c $< -lncursesw

clean:
	rm *.o game

.PHONY: clean
