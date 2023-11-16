#ifndef ERROR_DAEMON_H
#define ERROR_DAEMON_H

#include <bits/stdc++.h>
#include "../generics/Daemon.h"
#include "Keypress_Daemon.h"
using namespace std;

class Error_Daemon : public Daemon {
private:
    bool process_keystroke(queue<string>* errors, vector<Layer>* layers, int key) {
        switch (key) {
            case 269:
                subdaemon = new Keypress_Daemon(layers, 1, 
                        keystrokes_to_pass_down,
                        errors_from_down_and_self);
            default:
                key_to_pass_down = key;
                return true;
                break;
        }
        return true;
    }
    bool process_error(queue<string>* errors, vector<Layer>* layers, string error) {
        if (error.find("!!!END!!!") != string::npos) {
            delete subdaemon;
        }
        string s = "";
        if ((*layers)[starting_layer].check("ERRORS")) {
            s = (*layers)[starting_layer].objs.at("ERRORS").optional_string;
            (*layers)[starting_layer].remove(errors, "ERRORS");
        }
        s += error + "\n";
        (*layers)[starting_layer].add(errors, "ERRORS", *(new TGF_Object(errors, 0, 0, COLS, s)));
        return false;
    }
public:
    Error_Daemon(vector<Layer>* layers, int num_layers_in,
            queue<int>* keystrokes_from_up, queue<string>* errors_to_pass_up)
        : Daemon(layers, num_layers_in, keystrokes_from_up, errors_to_pass_up) {}
};

#endif
