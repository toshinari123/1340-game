#include <iostream>
#include <ncurses.h>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
using namespace std;
#define DELAY 30000
int game_Window_Size_X = 20;
int game_Window_Size_Y = 60;

class Monster{
public:
    int monsterx, monstery;
    string monster_string = "O";

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
        mvwprintw(window, monstery, monsterx, "%s", "  ");
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
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    //box(gameWindow, 0, 0);
    wrefresh(gameWindow);
    srand(time(NULL));
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
        int monster_ypos = rand()%19 + 1;
        int isMonsterCreate = rand()%50;
        if(isMonsterCreate == 1){
        Monster monster;
        monster.initialise(game_Window_Size_Y- 1, monster_ypos);
        monsters.push_back(monster);
        monster.draw(gameWindow);
        }
    //}
    if(!monsters.empty()){
   for(auto& monster : monsters){
       if(monster.monsterX() >10){
       monster.erase(gameWindow);
       monster.update(gameWindow);
       monster.draw(gameWindow);
       }
       if(monster.monsterX() == 10){
       monster.erase(gameWindow);
       }
   }
    }
    int input = getch();
    if(input == KEY_UP || input == KEY_DOWN){
    pistol.erase(gameWindow);
    pistol.updatePistol(input);
    pistol.draw(gameWindow);
    wrefresh(gameWindow);
    }
    if(input == KEY_LEFT){
        //printw("%s", "HIIIIII");
        Bullet bullet;
        bullet.initialise(8, pistol.pistolY() + 1);
        bullets.push_back(bullet);
        bullet.draw(gameWindow);
    }

    if(!bullets.empty()){
    for(auto& bullet : bullets){
    bullet.erase(gameWindow);
    bullet.update(gameWindow);
    bullet.draw(gameWindow);
    if(bullet.bulletX() == game_Window_Size_X -1){
        bullet.erase(gameWindow);
        wrefresh(gameWindow);
    }
    }
    wrefresh(gameWindow);
    }
    if(!bullets.empty() && !monsters.empty()){
    for(auto& monster : monsters){
        for(auto& bullet : bullets){
            if(bullet.bulletX() == monster.monsterX() && bullet.bulletY() == monster.monsterY()){
                bullet.erase(gameWindow);
                monster.erase(gameWindow);
            }
        }
    }
    }

    if(input == 'q' || input == 'Q'){
        break;
    }
    flushinp();
    usleep(DELAY);
    };
    getch();
    endwin();
return 0;
}
