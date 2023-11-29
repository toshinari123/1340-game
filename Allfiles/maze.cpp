#include <iostream>
#include <cstdlib>
#include <ctime>
#include <ncurses.h>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>

using namespace std;



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

void Game::window(){
    WINDOW* window= newwin(LINES,COLS, 5, COLS/2);
    printw("WELCOME TO THE MAZE OF MINIGAMES!\n");
    printw("Controls:\n");
    printw("Movement: Arrow Keys\n");
    printw("Goal: Get from the top left corner of the screen to the bottom left\n");
    printw("Be careful! If you hit a trap you have to play a minigame and will be sent to the begining\n");
    printw("Good Luck! Press Any Key to continue");
    getch();
    wrefresh(window);
    refresh();
    delwin(window);       

}

void Game:: playagain(){
    initscr(); // initialize ncurses
    raw(); // disable line buffering
    keypad(stdscr, TRUE); // enable special key input
    noecho(); // don't display input
    curs_set(0); // hide cursor
    start_color(); // enable color
    init_pair(1, COLOR_CYAN, COLOR_BLACK); // define color pair
    curs_set(0);
}

void Game::initGame() {
    playerX = 0;
    playerY = 0;
    endX = COLS - 1;
    endY = LINES - 1;

    system("cd ../Allfiles");
    system("make hangman");
    system("make main_gun_game");
    minigames[0]= "./hangman";
    minigames[1]= "./main_gun_game";
    minigames[2]= "./main_gun_game";

    for (int i = 0; i < numTraps; ++i) {
        trapX[i] = rand() % COLS;
        trapY[i] = rand() % LINES;
    }

    curs_set(0);
}

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
        mvprintw(trapY[i], trapX[i], "Q");
    }


    refresh();
}

void Game::getInput() {
    int key = getch();
    switch (key) {
        case KEY_UP:
            if (playerY > 0) playerY--;
            break;
        case KEY_DOWN:
            if (playerY < LINES - 1) playerY++;
            break;
        case KEY_LEFT:
            if (playerX > 0) playerX--;
            break;
        case KEY_RIGHT:
            if (playerX < COLS - 1) playerX++;
            break;
        case 'q':
            endwin(); 
            exit(0);
            break;
    }
}

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


void Game::showtrap(){
    for(int i=0; i<showX.size(); i++){
        mvprintw(showY[i], showX[i], "T");
    }
}

void Game::playMiniGame() {
    playerX=0;
    playerY=0;
    clear();
    int choose= rand() % 3;
    system(minigames[choose].c_str());
    refresh();
    playagain();
}

void Game::run() {
    while (true) {
        draw();
        getInput();
        update();
    }
}

int main() {
    Game game;
    game.run();

    return 0;
}
