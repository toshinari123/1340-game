#ifndef LAYER_H
#define LAYER_H

#include <bits/stdc++.h>
#include <ncurses.h>
#include "TGF_Object.h"
using namespace std;

class Layer {
private:
public:
    map<string, TGF_Object> objs;
    bool is_braille;
    map<int, map<int, int>> count;
    map<int, map<int, int>> screen;
    int layer_id;
    Layer(bool is_b, int layer_id_in) {
        is_braille = is_b;
        layer_id = layer_id_in;
    }
    void load_scene(queue<string>* errors, string p) {
        objs.clear();
        count.clear();
        screen.clear();
        is_braille = false;
        ifstream f;
        f.open(p);
        if (f.bad()) {
            errors -> push("Layer_load_scene: invalid scene file path " + p);
            return;
        }
        string s, file, name;
        int posx, posy, i = 0;
        getline(f, s);
        bool is_b;
        if (s == "is braille") is_braille = true;
        else if (s == "is not braille") is_braille = false;
        else {
            errors -> push("Layer_load_scene: first line in scene file " + p + " is not \"is braille\" or \"is not braille\"");
            return;
        }
        while (getline(f, s)) {
            i++;
            stringstream ss(s);
            if (!(ss >> file >> name)) {
                errors -> push("Layer_load_scene: error reading file and name in line " + to_string(i) + " of scene file " + p);
            }
            TGF_Object obj(errors, file, 0, 0, 0, 0);
            if (!(ss >> obj.alignx >> obj.aligny >> obj.offsetx >> obj.offsety)) {
                errors -> push("Layer_load_scene: error reading alignx, aligny, offsetx, offsety in line " + to_string(i) + " of scene file " + p);
            }
            if (is_braille) {
                if (!(ss >> obj.br_offsetx >> obj.br_offsety)) {
                    errors -> push("Layer_load_scene: error reading br_offsetx and br_offsety in line " + to_string(i) + " of scene file " + p);
                }
            }
            this -> add(errors, name, obj);
        }
    }
    ~Layer() {
        delete &objs;
    }
    void add(queue<string>* errors, string name, TGF_Object obj) {
        if (objs.find(name) != objs.end()) {
            errors -> push("Layer_add: duplicate name in layer");
        } else {
            int sx = int(double(LINES) * obj.alignx) + obj.offsetx;
            int sy = int(double(COLS) * obj.aligny) + obj.offsety;
            for (int i = 0; i < obj.bb_rows; i++) {
                for (int j = 0; j < obj.bb_cols; j++) {
                    if (obj.opacity[i * obj.bb_cols + j]) {
                        count[sx + i][sy + j]++;
                        if (count[sx + i][sy + j] > 1) {
                            errors -> push("Layer_add: layer " + to_string(layer_id) + " overlap in (" + to_string(sx + i) + ", " + to_string(sy + j) + ")");
                        }
                        screen[sx + i][sy + j] = obj.graphics[i * obj.bb_cols + j];
                    }
                }
            }
            objs.insert(make_pair(name, obj));
        }
    }
    void remove(queue<string>* errors, string name) {
        if (objs.find(name) == objs.end()) {
            errors -> push("Layer_remove: cannot find \"" + name + "\" in layer " + to_string(layer_id) + " to remove");
        } else {
            auto obj = objs.at(name);
            int sx = int(double(LINES) * obj.alignx) + obj.offsetx;
            int sy = int(double(COLS) * obj.aligny) + obj.offsety;
            for (int i = 0; i < obj.bb_rows; i++) {
                for (int j = 0; j < obj.bb_cols; j++) {
                    if (obj.opacity[i * obj.bb_cols + j]) {
                        count[sx + i][sy + j]--;
                        if (count[sx + i][sy + j] < 0) {
                            errors -> push("Layer_remove: layer " + to_string(layer_id) + " negative count in (" + to_string(sx + i) + ", " + to_string(sy + j) + ")");
                        }
                        screen[sx + i].erase(sy + j);
                    }
                }
            }
            objs.erase(name);
        }
    }
    void modify(queue<string>* errors, string name, TGF_Object new_obj) {
        this -> remove(errors, name);
        this -> add(errors, name, new_obj);
        /*if (objs.find(name) == objs.end()) {
            errors -> push("Layer_modify: cannot find \"" + name + "\" in layer to modify");
        } else {
            objs[name] = new_obj;
        }*/
    }
    //TODO: make another modify that takes in a function as input
    void redraw(queue<string>* errors) {
        screen.clear();
        count.clear();
        for (auto kv : objs) {
            TGF_Object obj = kv.second;
            int sx = int(double(LINES) * obj.alignx) + obj.offsetx;
            int sy = int(double(COLS) * obj.aligny) + obj.offsety;
            for (int i = 0; i < obj.bb_rows; i++) {
                for (int j = 0; j < obj.bb_cols; j++) {
                    if (obj.opacity[i * obj.bb_cols + j]) {
                        count[sx + i][sy + j]++;
                        if (count[sx + i][sy + j] > 1) {
                            errors -> push("Layer_redraw: layer " + to_string(layer_id) + " overlap in (" + to_string(sx + i) + ", " + to_string(sy + j) + ")");
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
    int render(int x, int y) {
        if (screen.find(x) == screen.end()) return -1;
        if (screen[x].find(y) == screen[x].end()) return -1;
        return screen[x][y];
    }
};

#endif
