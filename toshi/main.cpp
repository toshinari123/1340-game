#include <bits/stdc++.h>
#include <ncurses.h>
#include "generics/misc.h"
#include "daemons/Error_Daemon.h"
using namespace std;

vector<Layer>* layers;
thread* render_thread;
Daemon* ed;

const int MILLISECONDS_PER_FRAME = 10;

void render(vector<Layer>* layers) {
    while (true) {
        const auto now = chrono::steady_clock::now();
        set<pair<int, int>> s;
        for (int i = 0; i < LINES; i++) {
            for (int j = 0; j < COLS; j++) {
                if (!layers -> at(0).check(i, j)) s.insert(make_pair(i, j));
                else {
                    wchar_t temp = layers -> at(0).screen[i][j];
                    cchar_t ptr;
                    setcchar(&ptr, &temp, 0, 0, nullptr);
                    mvadd_wch(i, j, &ptr);
                }
            }
        }
        for (int l = 1; l < layers -> size(); l++) {
            set<pair<int, int>> temp_s;
            for (auto p : s) {
                if (layers -> at(l).check(p.first, p.second)) {
                    wchar_t temp = layers -> at(l).screen[p.first][p.second];
                    cchar_t ptr;
                    setcchar(&ptr, &temp, 0, 0, nullptr);
                    mvadd_wch(p.first, p.second, &ptr);
                    temp_s.insert(p);
                }
            }
            for (auto p : temp_s) s.insert(p);
        }
        this_thread::sleep_until(now + *(new chrono::duration<int, std::milli>(MILLISECONDS_PER_FRAME)));
    }
}

void handle_int(int sig) {
    signal(SIGINT, SIG_IGN);
    //doesnt work :(
    clear();
    mvaddstr(LINES / 2, COLS / 2 - 5, "EXITING...");
    this_thread::sleep_for(chrono::milliseconds(500));
    endwin();
    exit(0);
}

void handle_winch(int sig) {
    signal(SIGWINCH, SIG_IGN);
    endwin();
    initscr();
    refresh();
    clear();
    ed -> redraw();
    render_thread = new thread(render, layers);
    signal(SIGWINCH, handle_winch);
}

int main() {
    setlocale(LC_CTYPE, "");
    initscr();
    signal(SIGWINCH, handle_winch);
    signal(SIGINT, handle_int);
    noecho();
    cbreak();
    nonl();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    queue<int>* keypress = new queue<int>;
    queue<string>* error_daemon_errors = new queue<string>;
    layers = new vector<Layer>;
    ed = new Error_Daemon(layers, 1, keypress, error_daemon_errors);
    render_thread = new thread(render, layers);
    while (true) {
        int temp = getch();
        if (temp != ERR) keypress -> push(temp);
        if (!error_daemon_errors -> empty()) {
            render_thread -> detach();
            delete render_thread;
            if (error_daemon_errors -> size() == 1 && error_daemon_errors -> front() == "!!!END!!!") {
                break;
            }
            mvaddwstr(0, 0, s2ws("ERROR ERROR:").c_str());
            int r = 1;
            while (!error_daemon_errors -> empty()) {
                mvaddwstr(r, 0, s2ws(error_daemon_errors -> front()).c_str());
                r++;
                error_daemon_errors -> pop();
            }
            break;
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
    delete layers;
    delete ed;
    delete keypress;
    delete error_daemon_errors;
    clear();
    endwin();
}
