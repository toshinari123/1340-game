#include <bits/stdc++.h>
using namespace std;

class Daemon {
private:
    bool process_keystroke(vector<Layer>::iterator layer_it, int key, queue<string>* errors) {
        switch key {
            //add cases
            case default:
                //modify this if necessary
                key_to_pass_down = key;
                return true;
                break;
        }
    }
    bool process_error(string error) {
        if (error.find("!!!END!!!") != string::npos) {
            delete subdaemon;
        }
        error_to_pass_up = "Generic_Daemon: " + error;
        return true;
    }
    void run(vector<Layer>::iterator layer_it,
            queue<int>* keystrokes_from_up,
            queue<string>* errors_to_pass_up) {
        while (true) {
            if (!keystrokes_from_up -> empty()) {
                if (process_keystroke(layer_it, keystrokes_from_up -> front(), errors_from_down_and_self)) {
                    keystrokes_to_pass_down -> push(key_to_pass_down);
                }
                keystrokes -> pop();
            }
            if (!errors_from_down_and_self -> empty()) {
                if (process_error(errors_from_down_and_self -> front())) {
                    errors_to_pass_up -> push(errors_to_pass_up);
                }
                errors_from_down -> pop();
            }
            //sleep?
        }
    }
    thread deamon_thread;
    queue<int>* keystrokes_to_pass_down;
    int key_to_pass_down;
    queue<string>* errors_from_down_and_self;
    string error_to_pass_up;
    Daemon* subdaemon;
public:
    Daemon(vector<Layer>::iterator layer_it,
            int layers,
            queue<int>* keystrokes_from_up,
            queue<string>* errors_to_pass_up) {
        daemon_thread = new thread(self -> run, layer_it, keystrokes_from_up, errors_to_pass_up);
    }
    queue<int>* request_keystrokes() {
        keystrokes_to_pass_down = new queue<int>;
        return keystrokes_to_pass_down;
    }
    queue<string>* request_errors() {
        errors_from_down_and_self = new queue<string>;
        return errors_from_down_and_self;
    }
    ~Daemon() {
        daemon_thread.terminate();
    }
}
