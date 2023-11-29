#include <bits/stdc++.h>
#include <ncurses.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "generics/misc.h"
#include "daemons/Info_Daemon.h"
using namespace std;

vector<Layer>* layers;
thread* render_thread;
Daemon* info_daemon;
bool stop;
bool nospace;
atomic_int daemon_to_terminate;
atomic_bool daemon_terminated;

const int MILLISECONDS_PER_FRAME = 10;

void render(vector<Layer>* layers) {
    map<int, map<int, wchar_t>> prev_screen;
    while (!stop) {
        const auto now = chrono::steady_clock::now();
        set<pair<int, int>> s;
        for (int i = 0; i < LINES; i++) {
            for (int j = 0; j < COLS; j++) {
                if (!layers -> at(0).check(i, j)) s.insert(make_pair(i, j));
                else {
                    wchar_t temp = layers -> at(0).screen[i][j];
                    wcerr << temp;
                    if (prev_screen.find(i) == prev_screen.end() || prev_screen[i].find(j) == prev_screen[i].end() || prev_screen[i][j] != temp) {
                        cchar_t ptr;
                        setcchar(&ptr, &temp, 0, 0, nullptr);
                        mvadd_wch(i, j, &ptr);
                        prev_screen[i][j] = temp;
                    }
                }
            }
        }
        try {
            for (int l = 1; l < layers -> size(); l++) {
                set<pair<int, int>> temp_s;
                for (auto p : s) {
                    if (layers -> at(l).check(p.first, p.second)) {
                        wchar_t temp = layers -> at(l).screen[p.first][p.second];
                        temp_s.insert(p);
                        if (prev_screen.find(p.first) == prev_screen.end() || prev_screen[p.first].find(p.second) == prev_screen[p.first].end() || prev_screen[p.first][p.second] != temp) {
                            cchar_t ptr;
                            setcchar(&ptr, &temp, 0, 0, nullptr);
                            if (nospace) {
                                mvadd_wch(p.first, p.second, &ptr);
                            }
                            temp_s.insert(p);
                            prev_screen[p.first][p.second] = temp;
                        }
                    }
                }
                for (auto p : temp_s) s.erase(p);
            }
        } catch (const std::out_of_range& e) {
            //cout << "Out of Range error.";
        }
        for (auto p : s) {
            wchar_t temp = L' ';
            if (prev_screen.find(p.first) == prev_screen.end() || prev_screen[p.first].find(p.second) == prev_screen[p.first].end() || prev_screen[p.first][p.second] != temp) {
                cchar_t ptr;
                setcchar(&ptr, &temp, 0, 0, nullptr);
                mvadd_wch(p.first, p.second, &ptr);
                prev_screen[p.first][p.second] = temp;
            }
        }
        this_thread::sleep_until(now + *(new chrono::duration<int, std::milli>(MILLISECONDS_PER_FRAME)));
    }
    wcerr << L"stoepd" << endl;
}

void handle_int(int sig) {
    signal(SIGINT, SIG_IGN);
    //doesnt work :(
    clear();
    mvaddstr(LINES / 2, COLS / 2 - 5, "EXITING...");
    //this_thread::sleep_for(chrono::milliseconds(500));
    endwin();
    exit(0);
}

void init() {
    initscr();
    noecho();
    cbreak();
    nonl();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

}

void handle_winch(int sig) {
    signal(SIGWINCH, SIG_IGN);
    stop = true;
    endwin();
    /*init();
    clear();
    printw("press enter when you are done resizing.");
    nodelay(stdscr, FALSE);
    getch();
    nodelay(stdscr, TRUE);*/
    cout << "press enter when you are done resizing.";
    string s;
    getline(cin, s);
    init();
    winsize ws;
    ioctl(0, TIOCGWINSZ, &ws);
    resizeterm(ws.ws_row, ws.ws_col);
    wcerr << LINES << L" " << COLS << endl;
    wcerr << LINES << L" " << COLS << endl;
    info_daemon -> redraw(LINES, COLS);
    stop = false;
    render_thread = new thread(render, layers);
    signal(SIGWINCH, handle_winch);
}

int main() {
    nospace = true;
    setlocale(LC_CTYPE, "");
    init();
    signal(SIGWINCH, handle_winch);
    signal(SIGINT, handle_int);
    queue<int>* keypress = new queue<int>;
    queue<wstring>* info_daemon_errors = new queue<wstring>;
    layers = new vector<Layer>;
    info_daemon = new Info_Daemon(1, layers, 2, keypress, info_daemon_errors);
    render_thread = new thread(render, layers);
    while (true) {
        int temp = getch();
        if (temp != ERR) keypress -> push(temp);
        if (!info_daemon_errors -> empty()) {
            stop = true;
            if (info_daemon_errors -> size() == 1 && info_daemon_errors -> front() == L"!!!END!!!") {
                break;
            }
            mvaddwstr(0, 0, s2ws("ERROR ERROR:").c_str());
            int r = 1;
            while (!info_daemon_errors -> empty()) {
                //mvaddwstr(r, 0, info_daemon_errors -> front().c_str());
                wcerr << info_daemon_errors -> front() << endl;
                r++;
                info_daemon_errors -> pop();
            }
            this_thread::sleep_for(chrono::milliseconds(1000));
            break;
        }
        //this_thread::sleep_for(chrono::milliseconds(1));
    }
    delete info_daemon;
    delete layers;
    delete keypress;
    delete info_daemon_errors;
    clear();
    endwin();
}
