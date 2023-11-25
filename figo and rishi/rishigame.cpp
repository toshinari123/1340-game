#include <iostream>
#include <cstdlib>
#include <ctime>
#include <ncurses.h>
#include <fstream>

using namespace std;

//initialize
const int rows = 30;
const int cols = 30;

class Game {
public:
    Game();
    void run();
private:
    int playerX, playerY;
    int endX, endY;
    int trapX[5], trapY[5];
    int numTraps;

    void initGame();
    void draw();
    void getInput();
    void update();
    bool checkCollision();
    void playMiniGame();
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

void Game::initGame() {
    playerX = 0;
    playerY = 0;
    endX = cols - 1;
    endY = rows - 1;

    numTraps = rand() % 10 + 7; // random number of traps 7-10

    for (int i = 0; i < numTraps; ++i) {
        trapX[i] = rand() % cols;
        trapY[i] = rand() % rows;
    }
}

void Game::draw() {
    clear();

    // Draw player
    attron(COLOR_PAIR(1));
    mvprintw(playerY, playerX, "@");
    attroff(COLOR_PAIR(1));

    // Draw end point
    mvprintw(endY, endX, "X");

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
            if (playerY < rows - 1) playerY++;
            break;
        case KEY_LEFT:
            if (playerX > 0) playerX--;
            break;
        case KEY_RIGHT:
            if (playerX < cols - 1) playerX++;
            break;
        case 'q':
            endwin(); // 
            exit(0);
            break;
    }
}

void Game::update() {
    if (playerX == endX && playerY == endY) {
        clear();
        mvprintw(rows / 2, cols / 2 - 4, "You Win!");
        refresh();
        getch();
        endwin();
        exit(0);
    }

    if (checkCollision()) {
        playMiniGame(); // Open a random game file
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
    
    int randomGame = rand() % 4 + 1; // Assuming you have game1, game2, game3, and game4
    string gameFileName = "game" + to_string(randomGame);
    ifstream gameFile(gameFileName);

    if (!gameFile.is_open()) {
        cerr << "Error: Could not open mini-game file " << gameFileName << endl;
        return;
    }

    clear();
    mvprintw(rows / 2, cols / 2 - 10, "Mini-Game: %s", gameFileName.c_str());
    refresh();
    
    // Simulate playing the mini-game (you may need to modify this part based on your actual mini-games)
    getch();

    // Close the mini-game file
    gameFile.close();

    // Remove the trap after completing the mini-game
    for (int i = 0; i < numTraps; ++i) {
        if (playerX == trapX[i] && playerY == trapY[i]) {
            trapX[i] = -1; // Set the trap position to an invalid value to mark it as removed
            trapY[i] = -1;
        }
    }
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
