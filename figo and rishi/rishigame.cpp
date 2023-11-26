#include <iostream>
#include <cstdlib>
#include <ctime>
#include <ncurses.h>
#include <fstream>
#include <cstring>
#include <string>

using namespace std;



class Game {
public:
    Game();
    void run();
private:
    int playerX, playerY;
    int endX, endY;
    int trapX[20], trapY[20];
    int numTraps;

    void initGame();
    void draw();
    void getInput();
    void update();
    bool checkCollision();
    void playMiniGame();
    void playagain();
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
    initGame();
}

void Game:: playagain(){
    initscr(); // initialize ncurses
    raw(); // disable line buffering
    keypad(stdscr, TRUE); // enable special key input
    noecho(); // don't display input
    curs_set(0); // hide cursor
    start_color(); // enable color
    init_pair(1, COLOR_CYAN, COLOR_BLACK); // define color pair
}

void Game::initGame() {
    playerX = 0;
    playerY = 0;
    endX = COLS - 1;
    endY = LINES - 1;

    numTraps = 20;

    for (int i = 0; i < numTraps; ++i) {
        trapX[i] = rand() % COLS;
        trapY[i] = rand() % LINES;
    }
}

void Game::draw() {
    clear();
    setlocale(LC_ALL, "");

    // Draw player
    mvprintw(playerY, playerX, L"🤠");

    // Draw end point
    mvprintw(endY, endX, "🚩");

    // Draw traps (invisible)
    for (int i = 0; i < numTraps; ++i) {
        mvprintw(trapY[i], trapX[i], "T");
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
        printw(to_string(numTraps).c_str()); 
    }
}

bool Game::checkCollision() {
    for (int i = 0; i < numTraps; ++i) {
        if (playerX == trapX[i] && playerY == trapY[i]) {
            return true;
        }
    }
    return false;
}

//this function needs editing!!
void Game::playMiniGame() {
    playerX=0;
    playerY=0;
    clear();
    system("./hangman");
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
