#include <bits/stdc++.h>
#include <ncurses.h>
#include "daemons/Error_Daemon.h"
using namespace std;

const int MILLISECONDS_PER_FRAME = 10;

void render() {
    while (true) {
        const auto now = chrono::steady_clock::now();

        this_thread::sleep_until(now + *(new chrono::duration<int, std::milli>(MILLISECONDS_PER_FRAME)));
    }
}

int main() {
    initscr();
    addstr("helloworld");
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    queue<int>* keypress = new queue<int>;
    queue<string>* error_daemon_errors = new queue<string>;
    vector<Layer>* layers = new vector<Layer>;
    addstr("wtf");
    Daemon* ed = new Error_Daemon(layers, 1, keypress, error_daemon_errors);
    addstr("pls");
    while (true) {
        addstr("hello");
        //keypress -> push(getch());
        addstr("hello");
        if (!error_daemon_errors -> empty()) {
            if (error_daemon_errors -> size() == 1 && error_daemon_errors -> front() == "!!!END!!!") {
                break;
            }
            mvaddstr(0, 0, "ERROR ERROR:");
            int r = 1;
            while (!error_daemon_errors -> empty()) {
                mvaddstr(r, 0, (error_daemon_errors -> front()).c_str());
                r++;
                error_daemon_errors -> pop();
            }
            break;
        }
    }
    delete layers;
    delete ed;
    delete keypress;
    delete error_daemon_errors;
    endwin();
}
