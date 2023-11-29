#include <iostream>
#include <ncurses.h>
#include "game.h"

using namespace std;

int main(){
    //cout << "check 1" << endl;
    initscr();
    //cout << "check 2" << endl;
    //Don't buffer input
    cbreak();
    //cout << "check 3" << endl;
    //Enable input from special keys
    //keypad(stdscr, TRUE);
    //Don't echo input
    //noecho();
    //Don't display cursor
    //curs_set(0);
    //Update screen initially (prevents getch() from clearing the screen)
    refresh();

    //Set getch() to non-blocking mode
    //nodelay(stdscr, TRUE);
    game_main();
    //game_main();
    getch();
    endwin();
return 0;
}
