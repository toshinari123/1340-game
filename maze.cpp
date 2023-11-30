#include <iostream>
#include <cstdlib>
#include <ctime>
#include <ncurses.h>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>

using namespace std;


//Game Class to make the program easier to organise
class Game {
public:
    Game();
    void run();
private:
    int playerX, playerY;
    int endX, endY;
    static const int numTraps=50;
    int trapX[numTraps], trapY[numTraps];
    vector<int>showX;
    vector<int>showY;
    string minigames[3];
    void window();
    void initGame();
    void draw();
    void getInput();
    void update();
    bool checkCollision();
    void playMiniGame();
    void playagain();
    void showtrap();
};

//Initializing the main game
Game::Game() {
    srand(time(0));
    initscr(); // initialize ncurses
    raw(); // disable line buffering
    keypad(stdscr, TRUE); // enable special key input
    noecho(); // don't display input
    curs_set(0); // hide cursor
    start_color(); // enable color
    init_pair(1, COLOR_CYAN, COLOR_BLACK); // define color pair
    window();
    initGame();
}

//Function to display initial window
void Game::window(){
    WINDOW* window= newwin(LINES,COLS, 0, COLS/2);
    printw("WELCOME TO THE MAZE OF MINIGAMES!\n\n");
    printw("Controls:\n");
    printw("UP:    W key\n");
    printw("DOWN:  S key\n");
    printw("LEFT:  A key\n");
    printw("RIGHT: D key\n");
    printw("Quit:  Q\n\n");
    printw("Goal: Get from the top left corner of the screen to the bottom right\n\n");
    printw("Be careful! If you hit a trap you have to play a minigame and will be sent to the begining\n\n");
    printw("Good Luck! Press Any Key to continue");
    getch();
    wrefresh(window);
    refresh();
    delwin(window);       

}

//Funntion to call main maze game after minigame has been played
void Game:: playagain(){
    initscr(); // initialize ncurses
    raw(); // disable line buffering
    curs_set(0); // hide cursor
    keypad(stdscr, TRUE); // enable special key input
    noecho(); // don't display input
    start_color(); // enable color
    init_pair(1, COLOR_CYAN, COLOR_BLACK); // define color pair
}

//Initialize player, traps and minigames
void Game::initGame() {
    playerX = 0;
    playerY = 0;
    endX = COLS - 1;
    endY = LINES - 1;

    system("cd ../Allfiles");
    system("make hangman");
    system("make main_gun_game");
    system("make blackjack");
    minigames[0]= "./hangman";
    minigames[1]= "./main_gun_game";
    minigames[2] = "./blackjack";

    for (int i = 0; i < numTraps; ++i) {
        trapX[i] = rand() % COLS;
        trapY[i] = rand() % LINES;
    }

    curs_set(0);
}

//Display player and Traps on the board
void Game::draw() {
    clear();
    showtrap();
    setlocale(LC_ALL, "");

    // Draw player
    mvprintw(playerY, playerX, "P");

    // Draw end point
    mvprintw(endY, endX, "X");

    // Draw traps (invisible)
    for (int i = 0; i < numTraps; ++i) {
        mvprintw(trapY[i], trapX[i], "");
    }


    refresh();
}

//Controlls for player
void Game::getInput() {
    int key = getch();
    switch (key) {
        case 119:
            if (playerY > 0) playerY--;
            break;
        case 115:
            if (playerY < LINES - 1) playerY++;
            break;
        case 97:
            if (playerX > 0) playerX--;
            break;
        case 100:
            if (playerX < COLS - 1) playerX++;
            break;
        case 'q':
            endwin(); 
            exit(0);
            break;
    }
}


//Win condition and play minigame condition
void Game::update() {
    if (playerX == endX && playerY == endY) {
        clear();
        mvprintw(LINES / 2, COLS / 2 - 4, "You Win!");
        refresh();
        getch();
        endwin();
        exit(0);
    }

    if (checkCollision()) {
        playMiniGame();
    }
}

//Checks if the player has landed on a trap and if they have displays the trap
bool Game::checkCollision() {
    for (int i = 0; i < numTraps; ++i) {
        if (playerX == trapX[i] && playerY == trapY[i]) {
            showX.push_back(trapX[i]);
            showY.push_back(trapY[i]);
            trapX[i] = -1;
            trapY[i] = -1;
            return true;
        }
    }
    for (int j=0; j<showX.size(); j++){
        if (playerX == showX[j] && playerY == showY[j]) return true;
    }
    return false;
}


//Shows all traps the player has already been on
void Game::showtrap(){
    for(int i=0; i<showX.size(); i++){
        mvprintw(showY[i], showX[i], "T");
    }
}

//Program to run the minigame
void Game::playMiniGame() {
    playerX=0;
    playerY=0;
    clear();
    int choose= rand() % 3;
    system(minigames[choose].c_str());
    refresh();
    playagain();
}

//main run function
void Game::run() {
    while (true) {
        draw();
        getInput();
        update();
    }
}

//main function
int main() {
    Game game;
    game.run();

    return 0;
}
