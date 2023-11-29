#include "Menu_Daemon.h"
#include "Main_Menu_Daemon.h"
#include "Select_Game_Menu_Daemon.h"
using namespace std;

void Select_Game_Menu_Daemon::postprocess(queue<wstring>* errors) {
    if (cur_index == 0) {

    } else if (cur_index == 1) {
        //TODO
    } else if (cur_index == 2) {
        //TODO
    } else {
        superdaemon -> subdaemon = new Main_Menu_Daemon(daemon_id, layers,
                superdaemon -> keystrokes_to_pass_down,
                superdaemon -> errors_from_down_and_self, superdaemon);
        superdaemon -> subdaemon_name = "Main_Menu_Daemon";
    }
}
Select_Game_Menu_Daemon::Select_Game_Menu_Daemon(int daemon_id_in, vector<Layer>* layers,
        queue<int>* keystrokes_from_up, queue<wstring>* errors_to_pass_up, Daemon* superdaemon)
    : Menu_Daemon(daemon_id_in, layers, 1,
            "scenes/Select_Game_Menu.scn", "Option", 
            keystrokes_from_up, errors_to_pass_up, superdaemon) {
}
