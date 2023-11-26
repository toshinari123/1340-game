#ifndef LAYER_H
#define LAYER_H

#include <bits/stdc++.h>
#include <ncurses.h>
#include "TGF_Object.h"
#include "misc.h"
using namespace std;

class Layer {
private:
public:
    map<string, TGF_Object> objs;
    bool is_braille;
    map<int, map<int, int>> count;
    map<int, map<int, wchar_t>> screen;
    int layer_id;
    vector<string> optional_keywords;
    map<string, vector<string>> optional_lists;
    Layer(bool is_b, int layer_id_in) {
        is_braille = is_b;
        layer_id = layer_id_in;
    }
    Layer(const Layer &l) {
        objs = l.objs;
        is_braille = l.is_braille;
        count = l.count;
        screen = l.screen;
        layer_id = l.layer_id;
    }
    Layer& operator=(const Layer& other) {
        if (this != &other) {
            objs = other.objs;
            is_braille = other.is_braille;
            count = other.count;
            screen = other.screen;
            layer_id = other.layer_id;
        }
        return *this;
    }
    void load_scene(queue<wstring>* errors, string p) {
        objs.clear();
        count.clear();
        screen.clear();
        is_braille = false;
        ifstream f;
        f.open(p);
        if (f.bad()) {
            errors -> push(L"Layer_load_scene: invalid scene file path " + s2ws(p));
            return;
        }
        string s, file, name;
        int posx, posy, i = 0;
        f >> s;
        bool is_b;
        if (s == "is_braille") is_braille = true;
        else if (s == "is_not_braille") is_braille = false;
        else {
            errors -> push(L"Layer_load_scene: first line in scene file " + s2ws(p) + L" is not \"is braille\" or \"is not braille\"");
            return;
        }
        while (f >> file >> name) {
            i++;
            if (file[0] == '#') {
                string s;
                getline(f, s);
                continue;
            }
            /*if (!(ss >> file >> name)) {
                errors -> push("Layer_load_scene: error reading file and name in line " + to_string(i) + " of scene file " + p);
            }*/
            TGF_Object obj(errors, file, 0, 0, 0, 0);
            //for (int i = 0; i < obj.graphics.size(); i++) wcerr << "hmph" << obj.graphics[i];
            if (!(f >> obj.alignx >> obj.aligny >> obj.offsetx >> obj.offsety)) {
                errors -> push(L"Layer_load_scene: error reading alignx, aligny, offsetx, offsety in line " + to_wstring(i) + L" of scene file " + s2ws(p));
            }
            if (is_braille) {
                if (!(f >> obj.br_offsetx >> obj.br_offsety)) {
                    errors -> push(L"Layer_load_scene: error reading br_offsetx and br_offsety in line " + to_wstring(i) + L" of scene file " + s2ws(p));
                }
            }
            for (string a : optional_keywords) {
                if (name.find(a) != string::npos) {
                    optional_lists[a].push_back(name);
                }
            }
            this -> add(errors, name, obj);
        }
    }
    ~Layer() {
        objs.clear();
        count.clear();
        screen.clear();
        optional_keywords.clear();
        optional_lists.clear();
    }
    void add(queue<wstring>* errors, string name, TGF_Object obj) {
        if (objs.find(name) != objs.end()) {
            errors -> push(L"Layer_add: duplicate name in layer");
        } else {
            int sx = int(double(LINES) * obj.alignx) + obj.offsetx - obj.pp_row;
            int sy = int(double(COLS) * obj.aligny) + obj.offsety - obj.pp_col;
            wcerr << endl << L"name: " << s2ws(name) << endl << L"cont: " << obj.optional_string << endl << L"size: " << obj.graphics.size() << endl;
            for (int i = 0; i < obj.bb_rows; i++) {
                for (int j = 0; j < obj.bb_cols; j++) {
                    if (i * obj.bb_cols + j >= obj.opacity.size()) {
                        errors -> push(L"Layer_add: \"" + s2ws(name) + L"\" in layer " + to_wstring(layer_id) + L" opacity length is " + to_wstring(obj.opacity.size()) + L", should be " + to_wstring(obj.bb_rows * obj.bb_cols));
                    } else {
                        if (obj.opacity[i * obj.bb_cols + j]) {
                            count[sx + i][sy + j]++;
                            if (count[sx + i][sy + j] > 1) {
                                errors -> push(L"Layer_add: layer " + to_wstring(layer_id) + L" overlap in (" + to_wstring(sx + i) + L", " + to_wstring(sy + j) + L")");
                            }
                            screen[sx + i][sy + j] = obj.graphics[i * obj.bb_cols + j];
                            //wcerr << screen[sx + i][sy + j];
                        }
                    }
                }
                //wcerr << endl;
            }
            objs.insert(make_pair(name, obj));
        }
    }
    void remove(queue<wstring>* errors, string name) {
        if (objs.find(name) == objs.end()) {
            errors -> push(L"Layer_remove: cannot find \"" + s2ws(name) + L"\" in layer " + to_wstring(layer_id) + L" to remove");
        } else {
            auto obj = objs.at(name);
            int sx = int(double(LINES) * obj.alignx) + obj.offsetx - obj.pp_row;
            int sy = int(double(COLS) * obj.aligny) + obj.offsety - obj.pp_col;
            for (int i = 0; i < obj.bb_rows; i++) {
                for (int j = 0; j < obj.bb_cols; j++) {
                    if (i * obj.bb_cols + j >= obj.opacity.size()) {
                        errors -> push(L"Layer_remove: \"" + s2ws(name) + L"\" in layer " + to_wstring(layer_id) + L" opacity length is " + to_wstring(obj.opacity.size()) + L", should be " + to_wstring(obj.bb_rows * obj.bb_cols));
                    } else {
                        if (obj.opacity[i * obj.bb_cols + j]) {
                            count[sx + i][sy + j]--;
                            if (count[sx + i][sy + j] < 0) {
                                errors -> push(L"Layer_remove: layer " + to_wstring(layer_id) + L" negative count in (" + to_wstring(sx + i) + L", " + to_wstring(sy + j) + L")");
                            }
                            screen[sx + i].erase(sy + j);
                        }
                    }
                }
            }
            objs.erase(name);
        }
    }
    void modify(queue<wstring>* errors, string name, TGF_Object new_obj) {
        this -> remove(errors, name);
        this -> add(errors, name, new_obj);
        /*if (objs.find(name) == objs.end()) {
            errors -> push("Layer_modify: cannot find \"" + name + "\" in layer to modify");
        } else {
            objs[name] = new_obj;
        }*/
    }
    //TODO: make another modify that takes in a function as input
    void redraw(queue<wstring>* errors) {
        screen.clear();
        count.clear();
        for (auto kv : objs) {
            TGF_Object obj = kv.second;
            int sx = int(double(LINES) * obj.alignx) + obj.offsetx - obj.pp_row;
            int sy = int(double(COLS) * obj.aligny) + obj.offsety - obj.pp_col;
            for (int i = 0; i < obj.bb_rows; i++) {
                for (int j = 0; j < obj.bb_cols; j++) {
                    if (obj.opacity[i * obj.bb_cols + j]) {
                        count[sx + i][sy + j]++;
                        if (count[sx + i][sy + j] > 1) {
                            errors -> push(L"Layer_redraw: layer " + to_wstring(layer_id) + L" overlap in (" + to_wstring(sx + i) + L", " + to_wstring(sy + j) + L")");
                        }
                        screen[sx + i][sy + j] = obj.graphics[i * obj.bb_cols + j];
                    }
                }
            }
        }
    }
    void clear() {
        objs.clear();
        count.clear();
        screen.clear();
    }
    bool check(string name) {
        return objs.find(name) != objs.end();
    }
    bool check(int x, int y) {
        if (screen.find(x) == screen.end()) return false;
        if (screen[x].find(y) == screen[x].end()) return false;
        return true;
    }
};

#endif
