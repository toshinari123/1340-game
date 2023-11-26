#include <unistd.h>
#include <ncurses.h>
#include <string>
#include <cstdlib>
#include <ctime>
#include "popup.h"
#include "game.h"
#include <iostream>

using namespace std;

int popup_main(){
    clear();
    WINDOW* popUp = newwin(25, 60, 0, 0);
    box(popUp, 0, 0);
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    mvwprintw(popUp, 12, 20, "%s", "YOU LOST!");
    mvwprintw(popUp, 14, 20 , "%s", "Your highest score was : ");
    mvwprintw(popUp, 14, 46, "%i", highScore);
    wrefresh(popUp);
    int input;
    cin >> input;
    //getch();
    //endwin();
    return 0;
}
