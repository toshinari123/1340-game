#include <bits/stdc++.h>
#include "Menu_Daemon.h"
#include "Select_Game_Menu_Daemon.h"
#include "Main_Menu_Daemon.h"
using namespace std;

void Main_Menu_Daemon::postprocess(queue<wstring>* errors) {
    if (cur_index == 0) {
        superdaemon -> subdaemon = new Select_Game_Menu_Daemon(daemon_id, layers,
                superdaemon -> keystrokes_to_pass_down,
                superdaemon -> errors_from_down_and_self, superdaemon);
        superdaemon -> subdaemon_name = "Select_Game_Menu_Daemon";
    } else if (cur_index == 1) {
        //TODO
    } else {
        errors -> push(L"!!!EXIT!!!");
    }
}

Main_Menu_Daemon::Main_Menu_Daemon(int daemon_id_in, vector<Layer>* layers,
        queue<int>* keystrokes_from_up, queue<wstring>* errors_to_pass_up, Daemon* superdaemon)
    : Menu_Daemon(daemon_id_in, layers, 1,
            "scenes/Main_Menu.scn", "Button",
            keystrokes_from_up, errors_to_pass_up, superdaemon) {
}