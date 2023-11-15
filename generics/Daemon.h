#ifndef DAEMON_H
#define DAEMON_H

#include <bits/stdc++.h>
#include "Layer.h"
using namespace std;

class Daemon {
private:
    //modify below 2 functions in your custom daemon
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
        }
        error_to_pass_up = "Generic_Daemon: " + error;
        return true;
    }
    //modify above 2 functions in your custom daemon
    void run(vector<Layer>* layers,
            queue<int>* keystrokes_from_up,
            queue<string>* errors_to_pass_up) {
        while (true) {
        addstr("thread");
            if (!keystrokes_from_up -> empty()) {
                if (process_keystroke(errors_to_pass_up, layers, keystrokes_from_up -> front())) {
                    keystrokes_to_pass_down -> push(key_to_pass_down);
                }
                keystrokes_from_up -> pop();
            }
            if (!errors_from_down_and_self -> empty()) {
                if (process_error(errors_to_pass_up, layers, errors_from_down_and_self -> front())) {
                    errors_to_pass_up -> push(error_to_pass_up);
                }
                errors_from_down_and_self -> pop();
            }
            //sleep?
        }
    }
    thread* daemon_thread;
    vector<Layer>* layers_ref;
protected:
    Daemon* subdaemon;
    queue<int>* keystrokes_to_pass_down;
    int key_to_pass_down;
    queue<string>* errors_from_down_and_self;
    string error_to_pass_up;
public:
    int starting_layer;
    int num_layers;
    Daemon(vector<Layer>* layers, int num_layers_in,
            queue<int>* keystrokes_from_up, queue<string>* errors_to_pass_up) {
        addstr("ctor");
        layers_ref = layers;
        starting_layer = layers -> size();
        num_layers = num_layers_in;
        keystrokes_to_pass_down = new queue<int>;
        errors_from_down_and_self = new queue<string>;
        for (int i = 0; i < num_layers; i++) {
            layers -> push_back(*(new Layer(false, layers -> size())));
        }
        daemon_thread = new thread(&Daemon::run, this, layers, 
                keystrokes_from_up, errors_to_pass_up);
    }
    ~Daemon() {
        for (int i = 0; i < num_layers; i++) layers_ref -> pop_back();
        delete subdaemon;
        delete daemon_thread;
    }
};

#endif
