#ifndef CREDITS_DAEMON_H
#define CREDITS_DAEMON_H

#include "../generics/Daemon.h"
#include "../generics/misc.h"
using namespace std;

class Credits_Daemon : public Daemon {
    bool process_keystroke(queue<wstring>* errors, int key) {
        if (key == 27) {
            wcerr<< L"hello?";
            daemon_to_terminate = daemon_id;
        }
        return true;
    }
    void postprocess(queue<wstring>* errors) {
        wcerr << L"porcesprocessing...";
            superdaemon -> subdaemon = new Main_Menu_Daemon(daemon_id, layers,
                superdaemon -> keystrokes_to_pass_down,
                superdaemon -> errors_from_down_and_self, superdaemon);
            superdaemon -> subdaemon_name = "Main_Menu_Daemon";
    }
public:
    Credits_Daemon(int daemon_id_in, vector<Layer>* layers, 
            queue<int>* keystrokes_from_up, queue<wstring>* errors_to_pass_up, Daemon* superdaemon)
        : Daemon(daemon_id_in, layers, 1, 
                keystrokes_from_up, errors_to_pass_up, superdaemon) {
        layers -> at(starting_layer).load_scene(errors_to_pass_up, "scenes/credits.scn");
        preprocessed = true;
    }
};

#endif


