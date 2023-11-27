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
    WINDOW* popUp = newwin(23, 85, 0, 0);
    box(popUp, 0, 0);
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    mvwprintw(popUp, 11, 36, "%s", "YOU LOST!");
    mvwprintw(popUp, 12, 32 , "%s", "Your score was : ");
    mvwprintw(popUp, 12, 49, "%i", score);
    mvwprintw(popUp, 13, 25 , "%s", "PRESS Q TO GO BACK TO THE MAIN GAME");

    wrefresh(popUp);
    char input;
    cin >> input;
    if(input == 'q' || input == 'Q'){
        clear();
        /* call the main game function here
        Example : game_main();
        */
        endwin();
    }
    endwin();
    //getch();
    //endwin();
    return 0;
}
