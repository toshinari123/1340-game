#include <iostream>
#include <ncurses.h>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "gun_game.h"
using namespace std;

#define DELAY 30000 //define the delay between each iteration in the main while loop

int game_Window_Size_X = 19;
int game_Window_Size_Y = 50;
int score = 0;
int lives = 10;

class Monster{
public:
    int monsterx, monstery, count = 0;
    string monster_string = "O";

    bool correct_shot = false;

    //what it does: initlises monster (like a constructor)
    //inputs: x position (monsterx), y position (monster y)
    //outputs: void
    void initialise(int monsterx, int monstery){
        this -> monsterx = monsterx;
        this -> monstery = monstery;
    }
    //what it does: updates window so the monster moves left by 1 character
    //inputs: window
    //outputs: void
    void update(WINDOW* window){
        monsterx--;
    }
    //what it does: draws monster in specified window
    //inputs: window
    //outputs: void
    void draw(WINDOW* window){
        mvwprintw(window, monstery, monsterx, "%s", monster_string.c_str());
    }
    //what it does: erases monster in specified window (draw a space on top)
    //inputs: window
    //outputs: void
    void erase(WINDOW* window){
        mvwprintw(window, monstery, monsterx, "%s", " ");
    }
    //what it does: uh a function to get the public variable monster x
    //inputs: no input
    //outputs: int monsterx
    int monsterX(){
        return monsterx;
    }
    //what it does: a function to get the public variable monster y
    //inputs: no input
    //outputs: int monstery
    int monsterY(){
        return monstery;
    }
};
class Bullet{
public:
    int bulletx, bullety;
    string bullet_string = "=";
    //what it does: initlises bullet (like a constructor)
    //inputs: x position (bullet x), y position (bullet y)
    //outputs: void
    void initialise(int bulletx, int bullety){
        this -> bulletx = bulletx;
        this -> bullety = bullety;
    }
    //what it does: draws bullet in specified window
    //inputs: window
    //outputs: void
    void draw(WINDOW* window){
        mvwprintw(window, bullety, bulletx,"%s",bullet_string.c_str());
    }
    //what it does: updates window so the bullet moves right by 1 character
    //inputs: window
    //outputs: void
    void update(WINDOW* window){
            //usleep(DELAY);
            bulletx += 1;
    }
    //what it does: erases bullet in specified window (draw a space on top)
    //inputs: window
    //outputs: void
    void erase(WINDOW* window){
        mvwprintw(window, bullety, bulletx, " ");
    }
    //similar to monsterX
    int bulletX(){
        return bulletx;
    }
    //similar to monsterY
    int bulletY(){
        return bullety;
    }

};
class Pistol{
public:
    int pistolx, pistoly;
    string pistol_string_1 = "_______";
    string pistol_string_2 = "| _____|";
    string pistol_string_3 = "|_|";
    //what it does: initlises pistol (like a constructor)
    //inputs: x position (pistolx), y position (pistoly)
    //outputs: void
    void initialise(int pistolx, int pistoly){
        this -> pistoly = pistoly;
        this -> pistolx = pistolx;
    }
    //what it does: draws pistol in specified window
    //inputs: window
    //outputs: void
    void draw(WINDOW* window) {
        mvwprintw(window, pistoly, pistolx,"%s",pistol_string_1.c_str());
        mvwprintw(window, pistoly + 1, pistolx,"%s",pistol_string_2.c_str());
        mvwprintw(window, pistoly + 2, pistolx,"%s",pistol_string_3.c_str());
    }
    //what it does: updates window so the pistol moves up or down
    //inputs: window
    //outputs: void
    void updatePistol(int input) {
        switch(input) {
            case 'w':
            case 'W':
                if(pistoly > 1) {
                    pistoly--;
                }
                break;
            case 's':
            case 'S':
                if(pistoly < game_Window_Size_X -5) {
                    pistoly++;
                }
                break;
            default:
                break;
        }
    }
    //similar to monsterY
    int pistolY(){
        return pistoly;
    }
    //what it does: erases pistol in specified window (draw spaces on top)
    //inputs: window
    //outputs: void
    void erase(WINDOW *window){
        mvwprintw(window, pistoly, pistolx,"%s", "       ");
        mvwprintw(window, pistoly + 1, pistolx,"%s", "        ");
        mvwprintw(window, pistoly + 2, pistolx,"%s", "       ");
    }
};


int main(){
    initscr();
    cbreak();
    refresh();
    bool isLost = false;
    bool isWin = false;

    WINDOW* gameWindow = newwin(game_Window_Size_X, game_Window_Size_Y, 3,0);
    WINDOW* scoreWindow = newwin(3, game_Window_Size_Y, 0,0);
    WINDOW* instructionsWindow = newwin(game_Window_Size_X + 3, 35, 0, 50);

    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    box(gameWindow, 0, 0);
    box(scoreWindow, 0, 0);
    box(instructionsWindow, 0, 0);
    srand(time(NULL));

    //const strings
    mvwprintw(gameWindow, 6, 9, "%s", "UH OH! YOU STEPPED ON A TRAP.");
    mvwprintw(gameWindow, 7, 9, "%s", "PLAY THIS GAME TO GO BACK AND ");
    mvwprintw(gameWindow, 8, 9, "%s", "FIND YOUR WAY OUT OF THE MAZE!");
    mvwprintw(gameWindow, 9, 9, "%s", "PRESS G TO START THE GAME");
    mvwprintw(scoreWindow, 1, 2, "%s", "Score:");
    mvwprintw(scoreWindow, 1, 40, "%s", "Lives:");
    mvwprintw(instructionsWindow, 1, 1, "%s", "GAME MANUAL:");
    mvwprintw(instructionsWindow, 2, 1, "%s", "Start : G");
    mvwprintw(instructionsWindow, 3, 1, "%s", "Move Pistol up : W");
    mvwprintw(instructionsWindow, 4, 1, "%s", "Move Pistol down : S");
    mvwprintw(instructionsWindow, 5, 1, "%s", "Shoot : SpaceBar");
    mvwprintw(instructionsWindow, 6, 1, "%s", "Quit : Q");
    mvwprintw(instructionsWindow, 7, 1, "%s", "-----------------------------");
    mvwprintw(instructionsWindow, 8, 1, "%s", "HOW TO PLAY:");
    mvwprintw(instructionsWindow, 9, 1, "%s", "Shoot the monsters with the ");
    mvwprintw(instructionsWindow, 10, 1, "%s", "pistol. Your score increments");
    mvwprintw(instructionsWindow, 11, 1, "%s", "by 3 everytime you hit a ");
    mvwprintw(instructionsWindow, 12, 1, "%s", "monster. You initially have ");
    mvwprintw(instructionsWindow, 13, 1, "%s", "10 lives and you will lose one");
    mvwprintw(instructionsWindow, 14, 1, "%s", "life everytime you miss the ");
    mvwprintw(instructionsWindow, 15, 1, "%s", "target.");
    mvwprintw(instructionsWindow, 18, 1, "%s", "Score 30 points to win!");

    //refresh all windows to display
    wrefresh(gameWindow);
    wrefresh(scoreWindow);
    wrefresh(instructionsWindow);
    
    //initialise pistol
    Pistol pistol;
    pistol.initialise(1,1);

    //initialise bullets and monsters
    vector<Bullet> bullets;
    vector<Monster> monsters;

    wrefresh(gameWindow);

    //handle condition for starting the game
    char pre_game_input;
    cin >> pre_game_input;
    bool start_game = false;

    if(pre_game_input == 'g' || pre_game_input == 'G'){
        mvwprintw(gameWindow, 6, 9, "%s", "                                 ");
        mvwprintw(gameWindow, 7, 9, "%s", "                                   ");
        mvwprintw(gameWindow, 8, 9, "%s", "                                  ");
        mvwprintw(gameWindow, 9, 9, "%s", "                                  ");
        start_game = true;
    }
    else if(pre_game_input == 'q' || pre_game_input == 'Q'){
        isLost = true;
    }
    else{
    while(pre_game_input != 'g' || pre_game_input != 'G' || pre_game_input != 'q' || pre_game_input != 'Q'){
        cin >> pre_game_input;
        if(pre_game_input == 'g' || pre_game_input == 'G'){
        mvwprintw(gameWindow, 6, 9, "%s", "                                 ");
        mvwprintw(gameWindow, 7, 9, "%s", "                                   ");
        mvwprintw(gameWindow, 8, 9, "%s", "                                  ");
        mvwprintw(gameWindow, 9, 9, "%s", "                                  ");
        start_game = true;
        break;
    }
    if(pre_game_input == 'q' || pre_game_input == 'Q'){
        isLost = true;
    }
    }
    }

    while(start_game){
        //score window updates
        mvwprintw(scoreWindow, 1, 9, "%s", "         ");
        mvwprintw(scoreWindow, 1, 9, "%i", score);
        mvwprintw(scoreWindow, 1, 47, "%s", "  ");
        mvwprintw(scoreWindow, 1, 47, "%i", lives);
        wrefresh(scoreWindow);

        //game window pistol print and monster initialisation
        pistol.draw(gameWindow);
        int monster_ypos = rand()%14 + 2;
        int isMonsterCreate = rand()%49;
        if(isMonsterCreate == 1){
        Monster monster;
           monster.initialise(game_Window_Size_Y- 2, monster_ypos);
           monsters.push_back(monster);
           monster.draw(gameWindow);
        }

    //handle monster updates
    if(!monsters.empty()){
        for(auto itm = monsters.begin(); itm != monsters.end(); ){
        Monster& monster = *itm;
            if(monster.correct_shot == false){
                //update monster if target is not hit
                if(monster.monsterX() >= 0){
                    monster.erase(gameWindow);
                    monster.update(gameWindow);
                    monster.draw(gameWindow);
	                if(monster.monsterX() <= 10) {
                        //handle monster reaching the boundary
                        pistol.draw(gameWindow);
                        box(gameWindow, 0, 0);
	                }
	                ++itm;
                } else if(monster.monsterX() < 2){
                if(lives > 0){
                    //decrease lives if monster is not hit
                    lives--;
                    if(lives == 0){
                        isLost = true;
                    }
                }

                //delete from vector
                monster.erase(gameWindow);
	            itm = monsters.erase(itm);
                } 
            }
        }

        if(isLost){
            break;
        }
    }

    //handle bullet initialisation
    int input = getch();
    if(input == 'w' || 'W' || 's' || 'S'){
    pistol.erase(gameWindow);
    pistol.updatePistol(input);
    pistol.draw(gameWindow);
    wrefresh(gameWindow);
    }
    if(input == ' '){
        Bullet bullet;
        bullet.initialise(8, pistol.pistolY() + 1);
        bullets.push_back(bullet);
        bullet.draw(gameWindow);
    }

    //handle bullet updates
    for(auto itb = bullets.begin(); itb != bullets.end(); ){
      //update bullet in each iteration
      Bullet& bullet = *itb;
      bullet.erase(gameWindow);
      bullet.update(gameWindow);
      bullet.draw(gameWindow);
      if(bullet.bulletX() >= game_Window_Size_Y -2){
        //handle bullet reaching the boundary
        bullet.erase(gameWindow);
	    itb = bullets.erase(itb);
      } else {
	    ++itb;
      }
    }

    //handle correct target being hit
    for(auto itb = bullets.begin(); itb != bullets.end(); ){
	Bullet& bullet = *itb;
	bool isHit = false;
        for(auto itm = monsters.begin(); itm != monsters.end(); ){
	    Monster& monster = *itm; 
            if((bullet.bulletX() >= monster.monsterX()) && bullet.bulletY() == monster.monsterY()){
                if(score < 10000){
                score += 3;
                }
                monster.correct_shot = true;
                bullet.erase(gameWindow);
                monster.erase(gameWindow);
		itm = monsters.erase(itm);
		isHit = true;
		break;
            } else {
		    ++itm;
	    }
        }
	if(isHit) {
	  itb = bullets.erase(itb);
	} else {
	  ++itb;
	}
    }

    //refresh window
    wrefresh(gameWindow);

    if(input == 'q' || input == 'Q'){
        isLost = true;
        break;
    }
    else if (score>=30){
        isWin = true;
        break;
    }
    flushinp();
    usleep(DELAY);
    };
    if(isWin && !isLost){
    isLost = false;
    WINDOW* popUp = newwin(23, 85, 0, 0);
    box(popUp, 0, 0);
    mvwprintw(popUp, 10, 36, "%s", "YOU WON!");
    mvwprintw(popUp, 11, 30 , "%s", "Your score was : ");
    mvwprintw(popUp, 11, 47, "%i", score);
    mvwprintw(popUp, 12, 25 , "%s", "PRESS Q TO GO BACK TO THE MAZE");

    wrefresh(popUp);
    char input;
    while(input != 'q' || input != 'Q'){
        cin >> input;
        if(input == 'q' || input == 'Q'){
            clear();
            /* call the main game function here
            Example : game_main();
            */
            break;
        }
    }
    endwin(); 
    }

    if(isLost && !isWin){
    isWin = false;
    WINDOW* popUp = newwin(23, 85, 0, 0);
    box(popUp, 0, 0);
    mvwprintw(popUp, 10, 36, "%s", "YOU LOST!");
    mvwprintw(popUp, 11, 30 , "%s", "Your score was : ");
    mvwprintw(popUp, 11, 47, "%i", score);
    mvwprintw(popUp, 12, 25 , "%s", "PRESS Q TO GO BACK TO THE MAZE");

    wrefresh(popUp);
    char input;
    while(input != 'q' || input != 'Q'){
        cin >> input;
        if(input == 'q' || input == 'Q'){
            clear();
            /* call the main game function here
            Example : game_main();
            */
            break;
        }
    }
    endwin();
    }
    getch();
    endwin();
return 0;
}
