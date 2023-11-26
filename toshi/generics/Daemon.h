#ifndef DAEMON_H
#define DAEMON_H

#include <bits/stdc++.h>
#include "Layer.h"
using namespace std;

extern atomic_int daemon_to_terminate;
extern atomic_bool daemon_terminated;

class Daemon {
private:
    //modify below 3 functions in your custom daemon
    virtual bool process_keystroke(queue<wstring>* errors, int key) {
        int smth = 0;
        if (key == smth) {
            //add cases
            return false;
        } else {
            //modify this if necessary
            key_to_pass_down = key;
            return true;
        }
        return true;
    }
    virtual bool process_error(queue<wstring>* errors, wstring error) {
        if (error.find(L"!!!END!!!") != string::npos) {
            delete subdaemon;
        }
        error_to_pass_up = L"Generic_Daemon: " + error;
        return true;
    }
    virtual void postprocess(queue<wstring>* errors) {
        //spawn a new daemon maybe
    }
    //modify above 3 functions in your custom daemon
    void run(queue<int>* keystrokes_from_up, queue<wstring>* errors_to_pass_up) {
        signal(SIGSEGV, daemon_handle_int);
        while (!preprocessed) {}
        while (daemon_to_terminate != daemon_id) {
            int dummy = keystrokes_from_up -> size();
            if (!(keystrokes_from_up -> empty())) {
                if (process_keystroke(errors_to_pass_up, keystrokes_from_up -> front())) {
                    keystrokes_to_pass_down -> push(key_to_pass_down);
                }
                keystrokes_from_up -> pop();
            }
            if (!(errors_from_down_and_self -> empty())) {
                if (process_error(errors_to_pass_up, errors_from_down_and_self -> front())) {
                    errors_to_pass_up -> push(error_to_pass_up);
                }
                errors_from_down_and_self -> pop();
            }
            //wcerr << daemon_id << L" alive\n";
        //this_thread::sleep_for(chrono::milliseconds(1));
        }
        //terminate_subdaemon();
        daemon_to_terminate = 0;
        wcerr << L"HRLLIHLIHIH: " << to_wstring(daemon_id) << L" OK\n";
        wcerr << L"What" << num_layers;
        for (int i = 0; i < num_layers; i++) {
            wcerr << L"YEY LAYER DLET\n";
            auto temp = layers -> begin() + starting_layer;
            layers -> erase(temp);
        }
        postprocess(errors_to_pass_up);
    }
    thread* daemon_thread;
protected:
    bool preprocessed;
    Daemon* superdaemon;
    int key_to_pass_down;
    wstring error_to_pass_up;
    vector<Layer>* layers;
    int starting_layer;
    int num_layers;
    int daemon_id;
public:
    Daemon* subdaemon;
    string subdaemon_name;
    queue<int>* keystrokes_to_pass_down;
    queue<wstring>* errors_from_down_and_self;
    Daemon(int daemon_id_in, vector<Layer>* layers_in, int num_layers_in,
            queue<int>* keystrokes_from_up, queue<wstring>* errors_to_pass_up,
            Daemon* superdaemon_in) {
        subdaemon_name = "no subdaemons";
        superdaemon = superdaemon_in;
        layers = layers_in;
        starting_layer = layers -> size();
        daemon_id = daemon_id_in;
        num_layers = num_layers_in;
        keystrokes_to_pass_down = new queue<int>;
        errors_from_down_and_self = new queue<wstring>;
        for (int i = 0; i < num_layers; i++) {
            Layer l(false, layers -> size());
            layers -> emplace_back(l);
        }
        daemon_thread = new thread(&Daemon::run, this, keystrokes_from_up, errors_to_pass_up);
    }
    Daemon(const Daemon &d) {
        daemon_thread = d.daemon_thread;
        layers = d.layers;
        subdaemon = d.subdaemon;
        keystrokes_to_pass_down = d.keystrokes_to_pass_down;
        errors_from_down_and_self = d.errors_from_down_and_self;
        starting_layer = d.starting_layer;
        num_layers = d.num_layers;
    }
    Daemon& operator=(const Daemon& d) {
        if (this != &d) {
            daemon_thread = d.daemon_thread;
            layers = d.layers;
            subdaemon = d.subdaemon;
            keystrokes_to_pass_down = d.keystrokes_to_pass_down;
            errors_from_down_and_self = d.errors_from_down_and_self;
            starting_layer = d.starting_layer;
            num_layers = d.num_layers;
        }
        return *this;
    }
    ~Daemon() {
        if (subdaemon_name != "no subdaemons") {
            delete subdaemon;
            while (subdaemon_name != "no subdaemons") {

            }
        }
        daemon_to_terminate = daemon_id;
    }
    /*void terminate_only_cur_daemon() {
        daemon_terminated = false;
        daemon_to_terminate = daemon_id;
        const auto now = chrono::steady_clock::now();
        while (!daemon_terminated && chrono::steady_clock::now().time_since_epoch().count() - now.time_since_epoch().count() < 1000000000) {
            wcerr << to_wstring(chrono::steady_clock::now().time_since_epoch().count() - now.time_since_epoch().count()) << endl;
        }
        wcerr << L"AAA" << daemon_terminated << L"AAA\n";
        if (daemon_terminated) {
            wcerr << L"HRLLIHLIHIH: " << to_wstring(daemon_id) << L" OK\n";
            daemon_to_terminate = 0;
            daemon_terminated = false;
            wcerr << L"What" << num_layers;
            for (int i = 0; i < num_layers; i++) {
                wcerr << L"YEY LAYER DLET\n";
                auto temp = layers -> begin() + starting_layer;
                layers -> erase(temp);
            }
            return;
        }
        errors_from_down_and_self -> push(L"daemon " + to_wstring(daemon_id) + L" took too long to terminate");
    }*/
    void redraw() {
        for (int i = 0; i < num_layers; i++) {
            layers -> at(starting_layer + i).redraw(errors_from_down_and_self);
        }
        if (subdaemon_name != "no subdaemons") subdaemon -> redraw();
    }
    static void daemon_handle_int(int sig) {
        signal(SIGSEGV, SIG_IGN);
        terminate();
    }
};

#endif
