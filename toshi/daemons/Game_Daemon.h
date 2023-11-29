#ifndef GAME_DAEMON_H
#define GAME_DAEMON_H

#include "../generics/Daemon.h"
#include "../generics/misc.h"
using namespace std;

class Game_Daemon : public Daemon {
private:
    void run(queue<int>* keystrokes_from_up, queue<wstring>* errors_to_pass_up) {
        //stop = true;
        system("./games/maze/game");
    }
public:
    Game_Daemon(int daemon_id_in, vector<Layer>* layers, 
            queue<int>* keystrokes_from_up, queue<wstring>* errors_to_pass_up, Daemon* superdaemon)
        : Daemon(daemon_id_in, layers, 1, 
                keystrokes_from_up, errors_to_pass_up, superdaemon) {
    }
};

#endif

