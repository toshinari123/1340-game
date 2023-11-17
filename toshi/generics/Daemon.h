#ifndef DAEMON_H
#define DAEMON_H

#include <bits/stdc++.h>
#include "Layer.h"
using namespace std;

class Daemon {
private:
    //modify below 2 functions in your custom daemon
    virtual bool process_keystroke(queue<string>* errors, vector<Layer>* layers, int key) {
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
    virtual bool process_error(queue<string>* errors, vector<Layer>* layers, string error) {
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
        //this_thread::sleep_for(chrono::milliseconds(1));
        }
    }
    thread* daemon_thread;
    vector<Layer>* layers_ref;
    mutex layers_mutex;
protected:
    Daemon* subdaemon;
    queue<int>* keystrokes_to_pass_down;
    int key_to_pass_down;
    queue<string>* errors_from_down_and_self;
    string error_to_pass_up;
public:
    int starting_layer;
    int num_layers;
    bool subdaemon_active;
    Daemon(vector<Layer>* layers, int num_layers_in,
            queue<int>* keystrokes_from_up, queue<string>* errors_to_pass_up) {
        subdaemon_active = false;
        layers_ref = layers;
        starting_layer = layers -> size();
        num_layers = num_layers_in;
        keystrokes_to_pass_down = new queue<int>;
        errors_from_down_and_self = new queue<string>;
        for (int i = 0; i < num_layers; i++) {
            Layer l(false, layers -> size());
            lock_guard<mutex> guard(layers_mutex);
            layers -> emplace_back(l);
        }
        daemon_thread = new thread(&Daemon::run, this, layers, 
                keystrokes_from_up, errors_to_pass_up);
    }
    Daemon(const Daemon &d) {
        daemon_thread = d.daemon_thread;
        layers_ref = d.layers_ref;
        subdaemon = d.subdaemon;
        keystrokes_to_pass_down = d.keystrokes_to_pass_down;
        errors_from_down_and_self = d.errors_from_down_and_self;
        starting_layer = d.starting_layer;
        num_layers = d.num_layers;
    }
    Daemon& operator=(const Daemon& d) {
        if (this != &d) {
            daemon_thread = d.daemon_thread;
            layers_ref = d.layers_ref;
            subdaemon = d.subdaemon;
            keystrokes_to_pass_down = d.keystrokes_to_pass_down;
            errors_from_down_and_self = d.errors_from_down_and_self;
            starting_layer = d.starting_layer;
            num_layers = d.num_layers;
        }
        return *this;
    }
    ~Daemon() {
        for (int i = 0; i < num_layers; i++) layers_ref -> pop_back();
        delete subdaemon;
        delete daemon_thread;
    }
    void redraw() {
        for (int i = 0; i < num_layers; i++) {
            layers_ref -> at(starting_layer + i).redraw(errors_from_down_and_self);
        }
        if (subdaemon_active) subdaemon -> redraw();
    }
};

#endif
