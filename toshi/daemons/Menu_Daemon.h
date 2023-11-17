#ifndef MENU_DAEMON_H
#define MENU_DAEMON_H

#include <bits/stdc++.h>
#include "../generics/Daemon.h"
using namespace std;

class Menu_Daemon : public Daemon {
private:
    bool process_keystroke(queue<string>* errors, vector<Layer>* layers, int key) {
        switch (key) {
            //add cases
            default:
                //modify this if necessary
                key_to_pass_down = key;
                return true;
                break;
        }
        return true;
    }
    bool process_error(queue<string>* errors, vector<Layer>* layers, string error) {
        if (error.find("!!!END!!!") != string::npos) {
            delete subdaemon;
            return false;
        }
        return true;
    }
public:
    Menu_Daemon(vector<Layer>* layers, int num_layers_in,
            queue<int>* keystrokes_from_up, queue<string>* errors_to_pass_up)
        : Daemon(layers, num_layers_in, keystrokes_from_up, errors_to_pass_up) {
        layers -> at(starting_layer).load_scene(errors_to_pass_up, "scenes/Main_Menu.scn");
    }
};

#endif
