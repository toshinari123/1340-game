#ifndef GAME_DAEMON_H
#define GAME_DAEMON_H

#include "../generics/Daemon.h"
#include "../generics/misc.h"
#include "Main_Menu_Daemon.h"
using namespace std;

class Game_Daemon : public Daemon {
private:
    void run(queue<int>* keystrokes_from_up, queue<wstring>* errors_to_pass_up) {
        //stop = true;
        system("./games/maze/game");
        //system("cd ../Allfiles && make clean && make maze && ./maze");
            superdaemon -> subdaemon = new Main_Menu_Daemon(daemon_id, layers,
                superdaemon -> keystrokes_to_pass_down,
                superdaemon -> errors_from_down_and_self, superdaemon);
            superdaemon -> subdaemon_name = "Main_Menu_Daemon";

    }
public:
    Game_Daemon(int daemon_id_in, vector<Layer>* layers, 
            queue<int>* keystrokes_from_up, queue<wstring>* errors_to_pass_up, Daemon* superdaemon)
        : Daemon(daemon_id_in, layers, 1, 
                keystrokes_from_up, errors_to_pass_up, superdaemon) {
    }
};

#endif

