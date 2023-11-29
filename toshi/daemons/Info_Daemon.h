#ifndef ERROR_DAEMON_H
#define ERROR_DAEMON_H

#include <bits/stdc++.h>
#include "../generics/Daemon.h"
#include "Main_Menu_Daemon.h"
using namespace std;

class Info_Daemon : public Daemon {
private:
    bool display_debug;
    bool process_keystroke(queue<wstring>* errors, int key) {
        if (display_debug) {
            if ((*layers)[starting_layer + 1].check("keystroke")) {
                (*layers)[starting_layer + 1].remove(errors, "keystroke");
            }
            wstring wcs = L" ";
            wcs[0] = wchar_t(key);
            wstring out = to_wstring(key) + L": " + wcs;
            if (key < 10) {
                out += L"   ";
            } else if (key < 100) {
                out += L"  ";
            } else if (key < 256) {
                out += L" ";
            }
            (*layers)[starting_layer + 1].add(errors, "keystroke", *(new TGF_Object(errors, 0.5, 0.5, out)));
        }
        if (key == KEY_F(5)) {
            if (!display_debug) {
                TGF_Object grid(errors, 0, 0, L"");
                grid.bb_rows = LINES;
                grid.bb_cols = COLS;
                grid.alignx = grid.aligny = grid.offsetx = grid.offsety = 0;
                for (int i = 0; i < LINES; i++) {
                    for (int j = 0; j < COLS; j++) {
                        grid.graphics.push_back((i % 5 == 0 && j % 5 == 0 && i != LINES / 2) ? L'+' : L' ');
                        grid.opacity.push_back((i % 5 == 0 && j % 5 == 0 && i != LINES / 2));
                    }
                }
                (*layers)[starting_layer + 1].add(errors, "grid", grid);
            } else {
                if ((*layers)[starting_layer + 1].check("keystroke")) {
                    (*layers)[starting_layer + 1].remove(errors, "keystroke");
                }
                (*layers)[starting_layer + 1].remove(errors, "grid");
            }
            display_debug = !display_debug;
            /*if (subdaemon_name != "Keypress_Daemon") {
                terminate_subdaemon();
                subdaemon = new Keypress_Daemon(daemon_id + 1, layers, 1, 
                        keystrokes_to_pass_down,
                        errors_from_down_and_self);
                subdaemon_name = "Keypress_Daemon";
            }*/
            key_to_pass_down = key;
        } else {
            key_to_pass_down = key;
        }
        return true;
    }
    bool process_error(queue<wstring>* errors, wstring error) {
        if (error.find(L"!!!END!!!") != string::npos) {
            delete subdaemon;
        }
        if (error.find(L"!!!EXIT!!!") != string::npos) {
            errors -> push(L"!!!END!!!");
            return false;
        }
        wstring s = L"";
        if ((*layers)[starting_layer].check("ERRORS")) {
            s = (*layers)[starting_layer].objs.at("ERRORS").optional_string;
            (*layers)[starting_layer].remove(errors, "ERRORS");
        }
        s += error + L"\n";
        int count = 0;
        for (wchar_t c : s) count += (c == L'\n');
        if (count > 3) {
            while (s[0] != L'\n') s.erase(s.begin());
            s.erase(s.begin());
        }
        TGF_Object err(errors, 0, 0, COLS, s);
        err.optional_string = s;
        (*layers)[starting_layer].add(errors, "ERRORS", err);
        return false;
    }
public:
    Info_Daemon(int daemon_id_in, vector<Layer>* layers, int num_layers_in,
            queue<int>* keystrokes_from_up, queue<wstring>* errors_to_pass_up)
        : Daemon(daemon_id_in, layers, num_layers_in, keystrokes_from_up, errors_to_pass_up, nullptr) {
        subdaemon = new Main_Menu_Daemon(daemon_id + 1, layers,
                keystrokes_to_pass_down, errors_from_down_and_self, this);
        subdaemon_name = "Main_Menu_Daemon";
        preprocessed = true;
    }
};

#endif
