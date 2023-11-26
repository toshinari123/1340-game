#include <iostream>
#include <ncurses.h>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "popup.h"
using namespace std;

#define DELAY 30000
int game_Window_Size_X = 20;
int game_Window_Size_Y = 70;
int highScore = 0;
int score = 0;
bool isLost = false;

class Monster{
public:
    int monsterx, monstery, count = 0;
    string monster_string = "O";

    bool correct_shot = false;

    void initialise(int monsterx, int monstery){
        this -> monsterx = monsterx;
        this -> monstery = monstery;
    }
    void update(WINDOW* window){
        monsterx--;
    }
    void draw(WINDOW* window){
        mvwprintw(window, monstery, monsterx, "%s",monster_string.c_str());
    }

    void erase(WINDOW* window){
        mvwprintw(window, monstery, monsterx, "%s", " ");
    }

    int monsterX(){
        return monsterx;
    }
    int monsterY(){
        return monstery;
    }
};
class Bullet{
public:
    int bulletx, bullety;
    string bullet_string = "=";

    void initialise(int bulletx, int bullety){
        this -> bulletx = bulletx;
        this -> bullety = bullety;
    }

    void draw(WINDOW* window){
        mvwprintw(window, bullety, bulletx,"%s",bullet_string.c_str());
    }

    void update(WINDOW* window){
            //usleep(DELAY);
            bulletx += 1;
    }

    void erase(WINDOW* window){
        mvwprintw(window, bullety, bulletx, " ");
    }

    int bulletX(){
        return bulletx;
    }
    int bulletY(){
        return bullety;
    }

};
class Pistol{
public:
    int pistolx, pistoly;
    string pistol_string = "_______\n| _____|\n|_|";

    void initialise(int pistolx, int pistoly){
        this -> pistoly = pistoly;
        this -> pistolx = pistolx;
    }

    void draw(WINDOW* window) {
        mvwprintw(window, pistoly, pistolx,"%s",pistol_string.c_str());

    }

    void updatePistol(int input) {
        switch(input) {
            case KEY_UP:
                if(pistoly > 0) {
                    pistoly--;
                }
                break;
            case KEY_DOWN:
                if(pistoly < game_Window_Size_X -7) {
                    pistoly++;
                }
                break;
            default:
                break;
        }
    }
    int pistolY(){
        return pistoly;
    }
    void erase(WINDOW *window){
            mvwprintw(window, pistoly, pistolx, "\n\n\n");
    }
};

int game_main(){
    WINDOW* gameWindow = newwin(game_Window_Size_X, game_Window_Size_Y, 0,0);
    WINDOW* scoreWindow = newwin(4, game_Window_Size_Y, 20,0);
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    //box(gameWindow, 0, 0);
    wrefresh(gameWindow);
    wrefresh(scoreWindow);
    srand(time(NULL));
    box(scoreWindow, 0, 0);
    Pistol pistol;
    pistol.initialise(0,0);
    pistol.draw(gameWindow);
    //Bullet bullet;
    vector<Bullet> bullets;
    vector<Monster> monsters;
        //printw("%s", "HIIIIII");
    wrefresh(gameWindow);
    while(true){
    //int monsters_num = rand()%4 + 1;
    //for(int i = 0; i < monsters_num; i++){
        mvwprintw(scoreWindow, 2, 2, "%s", "Score:");
        mvwprintw(scoreWindow, 2, 9, "%s", "         ");
        mvwprintw(scoreWindow, 2, 9, "%i", score);
        wrefresh(scoreWindow);
        int monster_ypos = rand()%14 + 1;
        int isMonsterCreate = rand()%50;
        if(isMonsterCreate == 1){
        Monster monster;
           monster.initialise(game_Window_Size_Y- 1, monster_ypos);
           monsters.push_back(monster);
           monster.draw(gameWindow);
        }
    //}
    if(!monsters.empty()){
   for(auto itm = monsters.begin(); itm != monsters.end(); ){
     Monster& monster = *itm;
     if(monster.correct_shot == false){
       if(monster.monsterX() >= 0){
         monster.erase(gameWindow);
         monster.update(gameWindow);
         monster.draw(gameWindow);
	 if(monster.monsterX() <= 10) {
            pistol.draw(gameWindow);
	 }
	 ++itm;
       } else if(monster.monsterX() < 0){
        if(score > 0){
         score--;
         if(score == 0){
            //mvwprintw(gameWindow, 5, 5, "%s", "AHHHHHHHHHHHHHHHHHHHHHHHHHHH");
            isLost = true;
         }
        }
         monster.erase(gameWindow);
	     itm = monsters.erase(itm);
       } 
     }
   }
   if(isLost){
    break;
   }
    }
    int input = getch();
    if(input == KEY_UP || input == KEY_DOWN){
    pistol.erase(gameWindow);
    pistol.updatePistol(input);
    pistol.draw(gameWindow);
    wrefresh(gameWindow);
    }
    if(input == ' '){
        //printw("%s", "HIIIIII");
        Bullet bullet;
        bullet.initialise(8, pistol.pistolY() + 1);
        bullets.push_back(bullet);
        bullet.draw(gameWindow);
    }

    /*if(!bullets.empty()){*/
    for(auto itb = bullets.begin(); itb != bullets.end(); ){
      Bullet& bullet = *itb;
      bullet.erase(gameWindow);
      bullet.update(gameWindow);
      bullet.draw(gameWindow);
      if(bullet.bulletX() >= game_Window_Size_Y -1){
        bullet.erase(gameWindow);
	itb = bullets.erase(itb);
      } else {
	++itb;
      }
    }
    wrefresh(gameWindow);
    /*}*/
    //if(!bullets.empty() && !monsters.empty()){
    for(auto itb = bullets.begin(); itb != bullets.end(); ){
	Bullet& bullet = *itb;
	bool isHit = false;
        for(auto itm = monsters.begin(); itm != monsters.end(); ){
	    Monster& monster = *itm; 
            if((bullet.bulletX() >= monster.monsterX()) && bullet.bulletY() == monster.monsterY()){
                if(score < 10000){
                score += 3;
                if(score > highScore){
                    highScore = score;
                }
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
    //}

    if(input == 'q' || input == 'Q'){
        break;
    }
    flushinp();
    usleep(DELAY);
    };
    if(isLost){
        popup_main();
    }
    getch();
    endwin();
return 0;
};
