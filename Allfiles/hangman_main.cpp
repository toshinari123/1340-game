#include <iostream>
#include <ncurses.h>
using namespace std;

#include "hangman.h"


int main() {
     // Call the function to initialize ncurses
    initialize_ncurses();
    
    //Show the welcomee screen
    welcome();

    //If not ready, exit
    if (!display_instructions()){
    printw("Thanks for considering the Hangman Adventure. Goodbye!\n");
    clear();
    cleanup_ncurses();
    }
    
    //If ready, clear the screen of instructions and start the game.
    else
    {
        clear();
        welcome();
        start_game();
    }
    return 0;
}
