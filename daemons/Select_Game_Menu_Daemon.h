#ifndef SELECT_GAME_MENU_DAEMON_H
#define SELECT_GAME_MENU_DAEMON_H

#include "Menu_Daemon.h"
class Select_Game_Menu_Daemon : public Menu_Daemon {
private:
    void postprocess(queue<wstring>* errors);
public:
    Select_Game_Menu_Daemon(int daemon_id_in, vector<Layer>* layers,
            queue<int>* keystrokes_from_up, queue<wstring>* errors_to_pass_up, Daemon* superdaemon);
};

#endif
