#include <bits/stdc++.h>
using namespace std;

class Layer {
private:
public:
    map<string, TGF_Object> objs;
    bool is_braille;
    Layer(bool is_b) {
        objs = *(new map<string, TGFObject>);
        is_braille = is_b;
    }
    Layer(string p, queue<string>* errors) {
        objs = *(new map<string, TGF_Object>);
        is_braille = false;
        ifstream f;
        f.open(p);
        if (f.bad()) {
            errors -> push("Layer_file_constructor: invalid scene file path " + p);
            return;
        }
        string s, file, name;
        int posx, posy, i = 0;
        getline(f, s);
        bool is_b;
        if (s == "is braille") is_braille = true;
        else if (s == "is not braille") is_braille = false;
        else {
            errors -> push("Layer_file_constructor: first line in scene file " + p + " is not \"is braille\" or \"is not braille\"");
            return;
        }
        while (getline(f, s)) {
            i++;
            stringstream ss(s);
            if (!(ss >> file >> name)) {
                errors -> push("Layer_file_constructor: error reading file and name in line " + to_string(i) + " of scene file " + p);
            }
            TGF_Object obj(file, errors);
            if (!(ss >> obj.alignx >> obj.aligny >> obj.offsetx >> obj.offsety)) {
                errors -> push("Layer_file_constructor: error reading alignx, aligny, offsetx, offsety in line " + to_string(i) + " of scene file " + p);
            }
            if (layer.is_braille) {
                if (!(ss >> obj.br_offsetx >> obj.br_offsety)) {
                    errors -> push("Layer_file_constructor: error reading br_offsetx and br_offsety in line " + to_string(i) + " of scene file " + p);
                }
            }
            self -> add(name, obj, errors);
        }
    }
    void add(string name, TGF_Object obj, queue<string>* errors) {
        if (objs.find(name) != objs.end()) {
            errors -> push("Layer_add: duplicate name in same layer");
        } else {
            objs[name] = obj;
        }
    }
    void modify(string name, TGF_Object new_obj, queue<string>* errors) {
        if (objs.find(name) == objs.end()) {
            errors -> push("Layer_modify: cannot find \"" + name + "\" in layer to modify");
        } else {
            objs[name] = new_obj;
        }
    }
    void remove(string name) {
        if (objs.find(name) == objs.end(), queue<string>* errors) {
            errors -> push("Layer_remove: cannot find \"" + name + "\" in layer to remove");
        } else {
            objs.remove(name);
        }
    }
    void render(queue<string>* errors) {

    }
}
