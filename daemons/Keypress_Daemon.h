#ifndef KEYPRESS_DAEMON_H
#define KEYPRESS_DAEMON_H

#include <bits/stdc++.h>
#include "../generics/Daemon.h"
using namespace std;

class Keypress_Daemon : public Daemon {
private:
    bool process_keystroke(queue<string>* errors, vector<Layer>* layers, int key) {
        if ((*layers)[starting_layer].check("keystroke")) {
            (*layers)[starting_layer].remove(errors, "keystroke");
        }
        (*layers)[starting_layer].add(errors, "keystroke", *(new TGF_Object(errors, 0.5, 0.5, to_string(key) + ": " + char(key))));
        return false;
    }
    bool process_error(queue<string>* errors, vector<Layer>* layers, string error) {
        return true;
    }
public:
    Keypress_Daemon(vector<Layer>* layers, int num_layers_in,
            queue<int>* keystrokes_from_up, queue<string>* errors_to_pass_up)
        : Daemon(layers, num_layers_in, keystrokes_from_up, errors_to_pass_up) {}
};

#endif
