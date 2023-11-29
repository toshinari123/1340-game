#ifndef MENU_DAEMON_H
#define MENU_DAEMON_H

#include "../generics/Daemon.h"
#include "../generics/misc.h"
using namespace std;

class Menu_Daemon : public Daemon {
private:
    vector<TGF_Object> focused, unfocused;
    bool process_keystroke(queue<wstring>* errors, int key) {
        if (cur_index < 0 || cur_index >= menu_seq.size()) cur_index = 0;
        int next_index = cur_index;
        if (key == 259) {
            next_index = (cur_index == 0 ? menu_seq.size() - 1 : cur_index - 1);
        } else if (key == 258) {
            next_index = (cur_index == menu_seq.size() - 1 ? 0 : cur_index + 1);
        } else if (key == 13) {
            daemon_to_terminate = daemon_id;
        } else {
            key_to_pass_down = key;
        }
        try {
            auto what1 = unfocused.size();
            auto what2 = focused.size();
            auto temp1 = unfocused[cur_index];
            auto temp2 = focused[next_index];
            layers -> at(starting_layer).objs.at(menu_seq[cur_index]) = temp1;
            layers -> at(starting_layer).objs.at(menu_seq[next_index]) = temp2;
            layers -> at(starting_layer).redraw(errors);
        } catch (const std::out_of_range& e) {
            //cout << "Out of Range error.";
        }
        cur_index = next_index;
        return true;
    }
    bool process_error(queue<wstring>* errors, wstring error) {
        if (error.find(L"!!!END!!!") != string::npos) {
            delete subdaemon;
            return false;
        }
        return true;
    }
public:
    int cur_index;
    vector<string> menu_seq;
    Menu_Daemon(int daemon_id_in, vector<Layer>* layers, int num_layers_in,
            string scn_file, string keyword,
            queue<int>* keystrokes_from_up, queue<wstring>* errors_to_pass_up, 
            Daemon* superdaemon)
        : Daemon(daemon_id_in, layers, num_layers_in, keystrokes_from_up, errors_to_pass_up, superdaemon) {
        cur_index = 0;
        layers -> at(starting_layer).optional_keywords.push_back(keyword);
        layers -> at(starting_layer).load_scene(errors_to_pass_up, scn_file);
        menu_seq = layers -> at(starting_layer).optional_lists[keyword];
        for (string cur : menu_seq) {
            TGF_Object temp = layers -> at(starting_layer).objs.at(cur);
            temp.load_file(errors_to_pass_up, "tgfs/" + cur + "_unfocused.tgf");
            unfocused.push_back(temp);
            temp.load_file(errors_to_pass_up, "tgfs/" + cur + "_focused.tgf");
            focused.push_back(temp);
            wcerr << L"sizes " << unfocused.size() << L" " << focused.size() << endl;
        }
        preprocessed = true;
    }
    ~Menu_Daemon() {
        focused.clear();
        unfocused.clear();
        menu_seq.clear();
    }
};

#endif
