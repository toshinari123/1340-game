#include <set>
#include <map>
#include <atomic>
#include <vector>
#include <string>
#include <queue>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <iostream>
#include <sstream>
#include <codecvt>
#include <locale>
#include <signal.h>
#include <ncurses.h>
#include <termios.h>
#include <sys/ioctl.h>
using namespace std;

bool stop;
bool nospace;
bool running_game;
int daemon_to_terminate;
bool daemon_terminated;

//what it does: convert string to wide string
//inputs: string
//output: wide string
wstring s2ws(const string& str) {
    using convert_typeX = codecvt_utf8<wchar_t>;
    wstring_convert<convert_typeX, wchar_t> converterX;
    return converterX.from_bytes(str);
}

//what it does: convert wide string to string
//inputs: wide string
//output: string
string ws2s(const wstring& wstr) {
    using convert_typeX = codecvt_utf8<wchar_t>;
    wstring_convert<convert_typeX, wchar_t> converterX;
    return converterX.to_bytes(wstr);
}

//there are daemons in this code which are classes that controls some number of layers
//each layer consists of non-overlapping objects, each daemon may have a subdaemon and
//a superdaemon
//
//the keystroke is passed down from daemons to subdaemons;
//errors are passed up from daemons to superdaemons.

//many of the following function has queue<wstring>* errors as the first argument.
//it is used as a way to pass errors up to its superdaemon until it reaches the topmost 
//info daemon at which point it will be displayed as error

//this class is basically a sprite that reads from a tgf file
class TGF_Object {
public:
    int bb_rows, bb_cols;
    int pp_row, pp_col;
    double alignx, aligny;
    int offsetx, offsety;
    int br_offsetx, br_offsety;
    vector<int> opacity, foreground, background;
    vector<wchar_t> graphics;
    bool is_braille;
    wstring optional_string;
private:
    //what it does: check each row of .tgf file if the width and the alphabet of each line is consistent
    //inputs: error queue, the row (s), the allowed characters (p), current row number (row), current tgf file path (path)
    //outputs: void, if it is inconsistent an error will be pushed
    void check_row(queue<wstring>* errors, wstring s, wstring p, int row, string path) {
        if (bb_cols == 0) bb_cols = s.size();
        if (bb_cols != s.size()) {
            errors -> push(L"TGF_Object_check_row: row length mismatch in row " + to_wstring(row) + L" in file " + s2ws(path));
            return;
        }
        for (wchar_t c : s) {
            int s = 0;
            if (p == L"*") s = 1;
            else for (wchar_t d : p) s += (c == d);
            if (s == 0) {
                errors -> push(L"TGF_Object_check_row: character is not one of " + p + L"in row " + to_wstring(row) + L" of file " + s2ws(path));
            }
        }
    }
public:
    //what it does: loads a .tgf file into a TGF_Object
    //inputs: errors queue, and the filepath of .tgf relative to where the user started the program
    //outputs: void
    void load_file(queue<wstring>* errors, string path) {
        bb_cols = bb_rows = pp_row = pp_col = 0;
        wifstream f;
        f.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
        f.open(path);
        if (!f.is_open()) {
            errors -> push(L"TGF_Object_file_constructor: invalid TGF_Object file path " + s2ws(path));
            return;
        }
        int cur_row = 0;
        wstring s;
        getline(f, s);
        //wcerr << L'[' << s << L']';
        if (s[s.size() - 1] == L'\r') s.pop_back();
        //wcerr << L'[' << s << L']';
        cur_row++;
        if (s != L"[position_point]") {
            errors -> push(L"TGF_Object_file_constructor: first row is not [position_point] in " + s2ws(path));
            return;
        }
        int count = 0;
        while (getline(f, s)) {
            if (s[s.size() - 1] == L'\r') s.pop_back();
            if (s.empty()) break;
            this -> check_row(errors, s, L"01", cur_row, path);
            cur_row++;
            for (int j = 0; j < s.size(); j++) {
                if (s[j] == L'1') {
                    count += 1, pp_row = bb_rows, pp_col = j;
                }
            }
            bb_rows++;
        }
        for (int i = 0; i < bb_rows; i++) {
            for (int j = 0; j < bb_cols; j++) {
                graphics.push_back(L' ');
                opacity.push_back(0);
                background.push_back(0);
                foreground.push_back(0);
            }
        }
        if (count == 0) {
            errors -> push(L"TGF_Object_file_constructor: no position point specified in " + s2ws(path));
        }
        if (count > 1) {
            errors -> push(L"TGF_Object_file_constructor: multiple position points specified in " + s2ws(path));
        }
        while (true) {
            if (!getline(f, s)) break;
            if (s[s.size() - 1] == L'\r') s.pop_back();
            wstring p = L"01";
            if (s[0] != L'[' || s[s.size() - 1] != L']') {
                errors -> push(L"TGF_Object_file_constructor: section heading format wrong in row " + to_wstring(cur_row) + L" of " + s2ws(path));
            }
            cur_row++;
            if (s == L"[graphics]") p = L"*";
            if (s == L"[foreground_color]" || s == L"[background_color]") p = L"BWrgbcym";
            wstring arr[bb_rows];
            for (int i = 0; i < bb_rows; i++) {
                cur_row++;
                getline(f, arr[i]);
                //wcerr << arr[i].size() << ": " << arr[i] << endl;
                if (arr[i].empty()) {
                    errors -> push(L"TGF_Object_file_constructor: missing line " + to_wstring(i) + L" of section " + s + L" in file " + s2ws(path));
                    break;
                }
                if (arr[i][arr[i].size() - 1] == L'\r') arr[i].pop_back();
                this -> check_row(errors, arr[i], p, cur_row, path);
            }
            if (s == L"[graphics]") {
                for (int i = 0; i < bb_rows; i++) {
                    for (int j = 0; j < bb_cols; j++) {
                        graphics[i * bb_cols + j] = arr[i][j];
                    }
                }
            } else if (s ==  L"[opacity]") {
                for (int i = 0; i < bb_rows; i++) {
                    for (int j = 0; j < bb_cols; j++) {
                        opacity[i * bb_cols + j] = (arr[i][j] == L'1');
                    }
                }
            } else if (s == L"[foreground_color]") {

            } else if (s == L"[background_color]") {

            } else if (s == L"[foreground_intensity]") {

            } else if (s == L"[background_intensity]") {

            }
            //work on colors later
            //https://www.linuxjournal.com/content/about-ncurses-colors-0
            if (!getline(f, s)) break;
            if (s[s.size() - 1] == L'\r') s.pop_back();
        }
        //wcerr << L"loaded: " << pp_row << L", " << pp_col << L"\n";
    }
    //what it does: constructs a TGF object from path and coordinates (alignment and offset)
    //inputs: path string, row alignment percentage (ax), col alignment percentage (ay), row offset (ox), col offsey (oy)
    //outputs: returns a TGF_Object
    TGF_Object(queue<wstring>* errors, string path, double ax, double ay, int ox, int oy) {
        alignx = ax;
        aligny = ay;
        offsetx = ox;
        offsety = oy;
        bb_cols = 0;
        bb_rows = 0;
        pp_col = 0;
        pp_row = 0;
        TGF_Object::load_file(errors, path);
    }
    //what it does: constructs a TGF object directly from a string
    //inputs: content (s), row offset (x), col offset (y), other arguments similar to printf (...)
    //outputs: a TGF_Object
    TGF_Object(queue<wstring>* errors, int x, int y, wstring s, ...) {
        alignx = 0;
        aligny = 0;
        offsetx = x;
        offsety = y;
        bb_cols = 0;
        bb_rows = 0;
        pp_col = 0;
        pp_row = 0;
        va_list vl;
        va_start(vl, s);
        wstring line;
        wchar_t out[10000];
        vswprintf(out, 10000, s.c_str(), vl);
        optional_string = out;
        is_braille = false;
        alignx = aligny = 0;
        wstringstream ss(s);
        bb_rows = bb_cols = pp_row = pp_col = 0;
        while (getline(ss, line)) {
            bb_rows++;
            bb_cols = max(bb_cols, int(line.size()));
        }
        for (int i = 0; i < bb_rows; i++) {
            for (int j = 0; j < bb_cols; j++) {
                graphics.push_back(L' ');
                opacity.push_back(0);
                background.push_back(0);
                foreground.push_back(0);
            }
        }
        int row = 0;
        wstringstream ss2(s);
        while (getline(ss2, line)) {
            for (int j = 0; j < line.size(); j++) {
                graphics[row * bb_cols + j] = line[j];
                opacity[row * bb_cols + j] = 1;
            }
        }
        va_end(vl);
    }
    //what it does: constructs a TGF object directly from a string, with alingment instead of offset
    //inputs: content (s), row alignment percentage (x), col alignment precentage (y), other arguments similar to printf (...)
    //outputs: a TGF_Object
    TGF_Object(queue<wstring>* errors, double x, double y, wstring s, ...) {
        alignx = x;
        aligny = y;
        offsetx = 0;
        offsety = 0;
        bb_cols = 0;
        bb_rows = 0;
        pp_col = 0;
        pp_row = 0;
        va_list vl;
        va_start(vl, s);
        wstring line;
        wchar_t out[10000];
        vswprintf(out, 10000, s.c_str(), vl);
        optional_string = out;
        is_braille = false;
        offsetx = offsety = 0;
        wstringstream ss(s);
        bb_rows = bb_cols = pp_row = pp_col = 0;
        while (getline(ss, line)) {
            bb_rows++;
            bb_cols = max(bb_cols, int(line.size()));
        }
        for (int i = 0; i < bb_rows; i++) {
            for (int j = 0; j < bb_cols; j++) {
                graphics.push_back(L' ');
                opacity.push_back(0);
                background.push_back(0);
                foreground.push_back(0);
            }
        }
        int row = 0;
        wstringstream ss2(s);
        while (getline(ss2, line)) {
            for (int j = 0; j < line.size(); j++) {
                graphics[row * bb_cols + j] = line[j];
                opacity[row * bb_cols + j] = 1;
            }
        }
        va_end(vl);
    }
    //what it does: constructs a TGF object directly from a string, with wrap if exceed a width
    //inputs: content (s), row offset (x), col offset (y), width, other arguments similar to printf (...)
    //outputs: a TGF_Object
    TGF_Object(queue<wstring>* errors, int x, int y, int width, wstring s, ...) {
        alignx = 0;
        aligny = 0;
        offsetx = x;
        offsety = y;
        bb_cols = 0;
        bb_rows = 0;
        pp_col = 0;
        pp_row = 0;
        va_list vl;
        va_start(vl, s);
        wstring line;
        wchar_t out[10000];
        vswprintf(out, 10000, s.c_str(), vl);
        optional_string = out;
        is_braille = false;
        alignx = aligny = 0;
        wstringstream ss(out);
        bb_rows = pp_row = pp_col = 0;
        bb_cols = width;
        while (getline(ss, line)) {
            bb_rows += line.size() / width + int((line.size() % width) > 0);
        }
        for (int i = 0; i < bb_rows; i++) {
            for (int j = 0; j < bb_cols; j++) {
                graphics.push_back(L' ');
                opacity.push_back(1);
                background.push_back(0);
                foreground.push_back(0);
            }
        }
        int row = 0;
        wstringstream ss2(s);
        while (getline(ss2, line)) {
            //wcerr << L"whyy " << line << endl;
            for (int i = 0; i <= line.size() / width; i++) {
                for (int j = 0; j < ((i == (line.size() / width)) ? line.size() % width : width); j++) {
                    graphics[row * width + j] = line[i * width + j];
                }
                row++;
            }
        }
        for (int i = 0; i < bb_rows; i++) {
            for (int j = 0; j < bb_cols; j++) {
                //wcerr << graphics[i * bb_cols + j];
            }
            //wcerr << endl;
        }
        //wcerr << L"eh\n";
        va_end(vl);
    }
    //what it does: destructor of TGF_Object
    ~TGF_Object() {
       opacity.clear();
       foreground.clear();
       background.clear();
       graphics.clear();
    }
    //what it does: copy constructor of TGF_object
    //inputs: TGF_Object
    //outputs: TGF_Object
    TGF_Object(const TGF_Object &o) {
        bb_rows = o.bb_rows;
        bb_cols = o.bb_cols;
        pp_row = o.pp_row;
        pp_col = o.pp_col;
        alignx = o.alignx;
        aligny = o.aligny;
        offsetx = o.offsetx;
        offsety = o.offsety;
        br_offsetx = o.br_offsetx;
        br_offsety = o.br_offsety;
        graphics = o.graphics;
        opacity = o.opacity;
        foreground = o.foreground;
        background = o.background;
        is_braille = o.is_braille;
        optional_string = o.optional_string;
    }
    //what it does: assignment operator for TGF_object
    //inputs: TGF_object
    //outputs: TGF_Object
    TGF_Object& operator=(const TGF_Object &o) {
        if (this != &o) {
            bb_rows = o.bb_rows;
            bb_cols = o.bb_cols;
            pp_row = o.pp_row;
            pp_col = o.pp_col;
            alignx = o.alignx;
            aligny = o.aligny;
            offsetx = o.offsetx;
            offsety = o.offsety;
            br_offsetx = o.br_offsetx;
            br_offsety = o.br_offsety;
            graphics = o.graphics;
            opacity = o.opacity;
            foreground = o.foreground;
            background = o.background;
            is_braille = o.is_braille;
            optional_string = o.optional_string;
        }
        return *this;
    }
    //what it does: moves TGF_Object by a certain difference in rows and difference in cols
    //inputs: difference in rows (dx), difference in cols (dy)
    //outputs: void
    void move_diff(int dx, int dy) {
        offsetx += dx;
        offsety += dy;
    }
    //what it does: moves TGF_Object by absolute coordinate (0, 0) is top left
    //inputs: rows (x), cols (y)
    //outputs: void
    void move_abs(int x, int y) {
        alignx = aligny = 0;
        offsetx = x;
        offsety = y;
    }
    //what it does: moves TGF_Object by percentage of screen height and screen width
    //inputs: row alignment percentage (ax), col alignment percentage (ay)
    //outputs: void
    void set_align(double ax, double ay) {
        alignx = ax;
        aligny = ay;
        offsetx = offsety = 0;
    }
    //what it does: sets all the coordinates to get the position you want
    //inputs: row alignment percentage (ax), col alignment percentage (ay), row offset (ox), col offset (oy)
    //outputs: void
    void set_all(double ax, double ay, int ox, int oy) {
        alignx = ax;
        aligny = ay;
        offsetx = ox;
        offsety = oy;
    }
};

//this Layer class contains the objects indexed by names, and accurate representation of current
//screen with a map of maps
class Layer {
private:
    int t_LINES, t_COLS;
public:
    map<string, TGF_Object> objs;
    bool is_braille;
    map<int, map<int, int>> count;
    map<int, map<int, wchar_t>> screen;
    int layer_id;
    vector<string> optional_keywords;
    map<string, vector<string>> optional_lists;
    //what it does: Layer constructor
    //inputs: is_braille (not used), layer_id (layer_id_in), initial screen height (linesss), initial screen width (colsss)
    //outputs: Layer
    Layer(bool is_b, int layer_id_in, int linesss, int colsss) : 
        is_braille(is_b), layer_id(layer_id_in), t_LINES(linesss), t_COLS(colsss) {
        //wcerr << L"aaaaaaaaaaaa" << t_LINES << t_COLS;
    }
    //what it does: Layer copy constructor 
    //inputs: Layer
    //outputs: Layer
    Layer(const Layer &l) {
        objs = l.objs;
        is_braille = l.is_braille;
        count = l.count;
        screen = l.screen;
        layer_id = l.layer_id;
        t_LINES = l.t_LINES;
        t_COLS = l.t_COLS;
    }
    //what it does: Layer assignment operator
    //inputs: Layer
    //outputs: Layer
    Layer& operator=(const Layer& other) {
        if (this != &other) {
            objs = other.objs;
            is_braille = other.is_braille;
            count = other.count;
            screen = other.screen;
            layer_id = other.layer_id;
        t_LINES = other.t_LINES;
        t_COLS = other.t_COLS;
        }
        return *this;
    }
    //what it does: loads a .scn file which contains a list of .tgf paths and their 4 coordinates
    //inputs: errors queue, .scn file path (p)
    //outputs: void
    void load_scene(queue<wstring>* errors, string p) {
        objs.clear();
        count.clear();
        screen.clear();
        is_braille = false;
        ifstream f;
        f.open(p);
        if (!f.is_open()) {
            //wcerr << L"failed";
            errors -> push(L"Layer_load_scene: invalid scene file path " + s2ws(p));
            return;
        }
        string s, file, name;
        int posx, posy, i = 0;
        /*f >> s;
        bool is_b;
        if (s == "is_braille") is_braille = true;
        else if (s == "is_not_braille") is_braille = false;
        else {
            errors -> push(L"Layer_load_scene: first line in scene file " + s2ws(p) + L" is not \"is braille\" or \"is not braille\"");
            return;
        }*/
        //wcerr << "what";
        //wcerr << s2ws(p);
        //wcerr << s2ws(s);
        while (f >> file >> name) {
        //wcerr << "what";
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
            //for (int i = 0; i < obj.graphics.size(); i++) //wcerr << "hmph" << obj.graphics[i];
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
    //what it does: Layer destructor
    ~Layer() {
        objs.clear();
        count.clear();
        screen.clear();
        optional_keywords.clear();
        optional_lists.clear();
    }
    //what it does: add an object to layer
    //inputs: errors queue, name, TGF_Object (obj)
    //outputs: void
    void add(queue<wstring>* errors, string name, TGF_Object obj) {
        if (objs.find(name) != objs.end()) {
            errors -> push(L"Layer_add: duplicate name in layer");
        } else {
        //wcerr << L"aaaaaaaaaaaa" << t_LINES << t_COLS;
            int sx = int(double(t_LINES) * obj.alignx) + obj.offsetx - obj.pp_row;
            int sy = int(double(t_COLS) * obj.aligny) + obj.offsety - obj.pp_col;
            //wcerr << endl << L"name: " << s2ws(name) << endl << L"cont: " << obj.optional_string << endl << L"size: " << obj.graphics.size() << endl;
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
                            ////wcerr << screen[sx + i][sy + j];
                        }
                    }
                }
                ////wcerr << endl;
            }
            objs.insert(make_pair(name, obj));
        }
    }
    //what it does: removes an object of layer
    //inputs: errors queue, name, 
    //outputs: void
    void remove(queue<wstring>* errors, string name) {
        if (objs.find(name) == objs.end()) {
            errors -> push(L"Layer_remove: cannot find \"" + s2ws(name) + L"\" in layer " + to_wstring(layer_id) + L" to remove");
        } else {
            auto obj = objs.at(name);
            int sx = int(double(t_LINES) * obj.alignx) + obj.offsetx - obj.pp_row;
            int sy = int(double(t_COLS) * obj.aligny) + obj.offsety - obj.pp_col;
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
    //what it does: modifies an object of current layer
    //inputs: errors queue, name, TGF_Object (new_obj)
    //outputs: void
    void modify(queue<wstring>* errors, string name, TGF_Object new_obj) {
        this -> remove(errors, name);
        this -> add(errors, name, new_obj);
        /*if (objs.find(name) == objs.end()) {
            errors -> push("Layer_modify: cannot find \"" + name + "\" in layer to modify");
        } else {
            objs[name] = new_obj;
        }*/
    }
    //what it does: redraw with new screen height and new screen width
    //inputs: errors queue, new screen height (lines), new screen width (cols)
    //outputs: void
    void redraw_with_params(queue<wstring>* errors, int lines, int cols) {
        //wcerr << L"what are you doing";
        t_LINES = lines, t_COLS = cols;
        //wcerr << L"redrawing layer " << layer_id << L":" << lines << L" " << cols << endl;
        redraw(errors);
    }
    //what it does: redraw all objects and updates the screen variable
    //inputs: errors queue
    //outputs: void
    void redraw(queue<wstring>* errors) {
        screen.clear();
        count.clear();
        //wcerr << L"frick " << t_LINES << L" " << t_COLS << endl;
        for (auto kv : objs) {
            TGF_Object obj = kv.second;
            int sx = int(double(t_LINES) * obj.alignx) + obj.offsetx - obj.pp_row;
            int sy = int(double(t_COLS) * obj.aligny) + obj.offsety - obj.pp_col;
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
    //what it does: clear all objects and screen
    //inputs: void
    //outputs: void
    void clear() {
        objs.clear();
        count.clear();
        screen.clear();
    }
    //what it does: check if an object with specific name is in this layer
    //inputs: name
    //outputs: true or false
    bool check(string name) {
        return objs.find(name) != objs.end();
    }
    //what it does: check if a character of a specific position exists in screen
    //inputs: row (x), col (y)
    //outputs: true or false
    bool check(int x, int y) {
        if (screen.find(x) == screen.end()) return false;
        if (screen[x].find(y) == screen[x].end()) return false;
        return true;
    }
};

//main Daemon class that every other Daemon is derived from
class Daemon {
private:
    /*void terminate_only_cur_daemon() {
        daemon_terminated = false;
        daemon_to_terminate = daemon_id;
        const auto now = chrono::steady_clock::now();
        while (!daemon_terminated && chrono::steady_clock::now().time_since_epoch().count() - now.time_since_epoch().count() < 1000000000) {
            //wcerr << to_wstring(chrono::steady_clock::now().time_since_epoch().count() - now.time_since_epoch().count()) << endl;
        }
        //wcerr << L"AAA" << daemon_terminated << L"AAA\n";
        if (daemon_terminated) {
            //wcerr << L"HRLLIHLIHIH: " << to_wstring(daemon_id) << L" OK\n";
            daemon_to_terminate = 0;
            daemon_terminated = false;
            //wcerr << L"What" << num_layers;
            for (int i = 0; i < num_layers; i++) {
                //wcerr << L"YEY LAYER DLET\n";
                auto temp = layers -> begin() + starting_layer;
                layers -> erase(temp);
            }
            return;
        }
        errors_from_down_and_self -> push(L"daemon " + to_wstring(daemon_id) + L" took too long to terminate");
    }*/
	virtual bool process_keystroke(queue<wstring>* errors, int key) {
        int smth = 0;
        if (key == smth) {
            //add cases
            return false;
        } else {
            //modify this if necessary
            key_to_pass_down = key;
            return true;
        }
        return true;
    }
    virtual bool process_error(queue<wstring>* errors, wstring error) {
        if (error.find(L"!!!END!!!") != string::npos) {
            delete subdaemon;
        }
        error_to_pass_up = L"Generic_Daemon: " + error;
        return true;
    }
    //what it does: overridable function to do some things just before the thread stops
    //inputs: errors queue
    //outputs: void
    virtual void postprocess(queue<wstring>* errors) {
        //spawn a new daemon maybe
    }
    //what it does: the function called by thread
    //inputs: keystrokes queue pointer, errors queue pointer
    //outputs: void
    virtual void run(queue<int>* keystrokes_from_up, queue<wstring>* errors_to_pass_up) {
        signal(SIGSEGV, daemon_handle_int);
        while (!preprocessed) {}
        while (daemon_to_terminate != daemon_id) {
            int dummy = keystrokes_from_up -> size();
            if (!(keystrokes_from_up -> empty())) {
                if (process_keystroke(errors_to_pass_up, keystrokes_from_up -> front())) {
                    keystrokes_to_pass_down -> push(key_to_pass_down);
                }
                keystrokes_from_up -> pop();
            }
            if (!(errors_from_down_and_self -> empty())) {
                if (process_error(errors_to_pass_up, errors_from_down_and_self -> front())) {
                    errors_to_pass_up -> push(error_to_pass_up);
                }
                errors_from_down_and_self -> pop();
            }
            ////wcerr << daemon_id << L" alive\n";
        //this_thread::sleep_for(chrono::milliseconds(1));
        }
        //terminate_subdaemon();
        daemon_to_terminate = 0;
        //wcerr << L"HRLLIHLIHIH: " << to_wstring(daemon_id) << L" OK\n";
        //wcerr << L"What" << num_layers;
        for (int i = 0; i < num_layers; i++) {
            //wcerr << L"YEY LAYER DLET\n";
            auto temp = layers -> begin() + starting_layer;
            layers -> erase(temp);
        }
        postprocess(errors_to_pass_up);
    }
    thread* daemon_thread;
protected:
    bool preprocessed;
    Daemon* superdaemon;
    int key_to_pass_down;
    wstring error_to_pass_up;
    vector<Layer>* layers;
    int starting_layer;
    int num_layers;
    int daemon_id;
public:
    Daemon* subdaemon;
    string subdaemon_name;
    queue<int>* keystrokes_to_pass_down;
    queue<wstring>* errors_from_down_and_self;
    //what it does: constructs Daemon
    //inputs: daemon_id_in, the vector of layers (layers_in), num_layers,
    //  pointer to queue of keystrokes from superdaemon, pointer to queue of errors from subdaemon
    //  pointer to superdaemon
    //outputs: Daemon
    Daemon(int daemon_id_in, vector<Layer>* layers_in, int num_layers_in,
            queue<int>* keystrokes_from_up, queue<wstring>* errors_to_pass_up,
            Daemon* superdaemon_in) {
        subdaemon_name = "no subdaemons";
        superdaemon = superdaemon_in;
        layers = layers_in;
        starting_layer = layers -> size();
        daemon_id = daemon_id_in;
        num_layers = num_layers_in;
        keystrokes_to_pass_down = new queue<int>;
        errors_from_down_and_self = new queue<wstring>;
        for (int i = 0; i < num_layers; i++) {
            //wcerr << L"rrgr " << LINES << COLS << endl;
            Layer l(false, layers -> size(), LINES, COLS);
            layers -> emplace_back(l);
        }
        daemon_thread = new thread(&Daemon::run, this, keystrokes_from_up, errors_to_pass_up);
    }
    //what it does: Daemon copy constructor
    //inputs: Daemon
    //outputs: Daemon
    Daemon(const Daemon &d) {
        daemon_thread = d.daemon_thread;
        layers = d.layers;
        subdaemon = d.subdaemon;
        keystrokes_to_pass_down = d.keystrokes_to_pass_down;
        errors_from_down_and_self = d.errors_from_down_and_self;
        starting_layer = d.starting_layer;
        num_layers = d.num_layers;
    }
    //what it does: Daemon assignment operator
    //inputs: Daemon
    //outputs: Daemon
    Daemon& operator=(const Daemon& d) {
        if (this != &d) {
            daemon_thread = d.daemon_thread;
            layers = d.layers;
            subdaemon = d.subdaemon;
            keystrokes_to_pass_down = d.keystrokes_to_pass_down;
            errors_from_down_and_self = d.errors_from_down_and_self;
            starting_layer = d.starting_layer;
            num_layers = d.num_layers;
        }
        return *this;
    }
    //what it does: Daemon destructor, also terminates subdaemons and stops their threads
    ~Daemon() {
        if (subdaemon_name != "no subdaemons") {
            //wcerr << L"waat: " << s2ws(subdaemon_name) << endl;
            delete subdaemon;
            while (subdaemon_name != "no subdaemons") {

            }
        }
        daemon_to_terminate = daemon_id;
        if (superdaemon != nullptr) superdaemon -> subdaemon_name = "no subdaemons";
    }
    //what it does: redraws layers handled by current daemon with new lines and cols
    //inputs: new screen height (lines), new screen width (cols)
    //outputs: void
    void redraw(int lines, int cols) {
        for (int i = 0; i < num_layers; i++) {
            layers -> at(starting_layer + i).redraw_with_params(errors_from_down_and_self, lines, cols);
        }
        if (subdaemon_name != "no subdaemons") subdaemon -> redraw(lines, cols);
    }
    //what it does: if the thread runs into a segmentation fault, terminates the whole program
    //inputs: signal (sig)
    //outputs: void
    static void daemon_handle_int(int sig) {
        signal(SIGSEGV, SIG_IGN);
        terminate();
    }
};

//Daemon that makes menus
class Menu_Daemon : public Daemon {
private:
    vector<TGF_Object> focused, unfocused;
    //what it does: process keystroke (arrow up down, w, s, enter, escape)
    //inputs: errors queue, the key to process
    //outputs: true to pass keystroke down, false to not
    bool process_keystroke(queue<wstring>* errors, int key) {
        if (cur_index < 0 || cur_index >= menu_seq.size()) cur_index = 0;
        int next_index = cur_index;
        if (key == 259 || key == 'w') {
            next_index = (cur_index == 0 ? menu_seq.size() - 1 : cur_index - 1);
        } else if (key == 258 || key == 's') {
            next_index = (cur_index == menu_seq.size() - 1 ? 0 : cur_index + 1);
        } else if (key == 27) {
            next_index = menu_seq.size() - 1;
            daemon_to_terminate = daemon_id;
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
    //what it does: process error
    //inputs: errors queue, the error to process
    //outputs: true to pass error up, false to catch the error and handle it
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
    //what it does: Menu_Daemon constructor
    //inputs: daemon_id, vector of layers, number of layers, path to .scn (scn_file)
    //  keyword, pointer to keystrokes queue, pointer to errors queue, pointer to superdaemon
    //outputs: a Menu_Daemon
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
            temp.load_file(errors_to_pass_up, cur + "_unfocused.tgf");
            unfocused.push_back(temp);
            temp.load_file(errors_to_pass_up, cur + "_focused.tgf");
            focused.push_back(temp);
            //wcerr << L"sizes " << unfocused.size() << L" " << focused.size() << endl;
        }
        preprocessed = true;
    }
    //what it does: Menu_Daemon destructor
    ~Menu_Daemon() {
        focused.clear();
        unfocused.clear();
        menu_seq.clear();
    }
};

//The daemon that runs the maze game
class Game_Daemon : public Daemon {
private:
    //what it does: overrided run function so the keystrokes are not processed by menu.cpp
    //inputs: pointer to keystrokes queue, pointer to errors queue
    //outputs: void
    void run(queue<int>* keystrokes_from_up, queue<wstring>* errors_to_pass_up) {
        running_game = true;
        system("./maze");
        running_game = false;
        //system("cd ../Allfiles && make clean && make maze && ./maze");
            /*superdaemon -> subdaemon = new Main_Menu_Daemon(daemon_id, layers,
                superdaemon -> keystrokes_to_pass_down,
                superdaemon -> errors_from_down_and_self, superdaemon);
            superdaemon -> subdaemon_name = "Main_Menu_Daemon";*/

errors_to_pass_up -> push(L"!!!EXIT!!!");
    }
public:
    //what it does: Game_Daemon constructor
    //inputs: daemon_id, pointer to vector of layers, pointer to keystrokes queue, pointer to 
    //  errors queue, pointer to superdaemon
    //outputs: a Game_Daemon
    Game_Daemon(int daemon_id_in, vector<Layer>* layers, 
            queue<int>* keystrokes_from_up, queue<wstring>* errors_to_pass_up, Daemon* superdaemon)
        : Daemon(daemon_id_in, layers, 1, 
                keystrokes_from_up, errors_to_pass_up, superdaemon) {
    }
};

//declration of Main_Menu_Daemon to avoid circular dependency (Main_Menu_daemon requires
//Credits_Daemon, Credits_Daemon requires Main_Menu_Daemon)
class Main_Menu_Daemon : public Menu_Daemon {
private:
    void postprocess(queue<wstring>* errors);
public:
    Main_Menu_Daemon(int daemon_id_in, vector<Layer>* layers,
            queue<int>* keystrokes_from_up, queue<wstring>* errors_to_pass_up, Daemon* superdaemon);
};

//daemon to show credits
class Credits_Daemon : public Daemon {
    //what it does: overrided keystroke function to handle escape
    //inputs: errors queue, key to process
    //outputs: bool
    bool process_keystroke(queue<wstring>* errors, int key) {
        if (key == 27) {
            //wcerr<< L"hello?";
            daemon_to_terminate = daemon_id;
        }
        return true;
    }
    //what it does: overrided posrprocesss function to spawn Main_Menu_Daemon after terminating
    //inputs: errors queue
    //outputs: void
    void postprocess(queue<wstring>* errors) {
        //wcerr << L"porcesprocessing...";
            superdaemon -> subdaemon = new Main_Menu_Daemon(daemon_id, layers,
                superdaemon -> keystrokes_to_pass_down,
                superdaemon -> errors_from_down_and_self, superdaemon);
            superdaemon -> subdaemon_name = "Main_Menu_Daemon";
    }
public:
    //what it does: Credits_Daemon constructor
    //inputs: daemon_id, pointer to vector of layers, pointer to queue of keystrokes,
    //  pointer to queue of errors, pointer to superdaemon
    //outputs: a Credits_Daemon
    Credits_Daemon(int daemon_id_in, vector<Layer>* layers, 
            queue<int>* keystrokes_from_up, queue<wstring>* errors_to_pass_up, Daemon* superdaemon)
        : Daemon(daemon_id_in, layers, 1, 
                keystrokes_from_up, errors_to_pass_up, superdaemon) {
        layers -> at(starting_layer).load_scene(errors_to_pass_up, "credits.scn");
        preprocessed = true;
    }
};

//what it does: overrided postprocess function to spawn different daemons depending on which
//  index currently in (or to exit the program)
//inputs: queue of errors
//outputs: void
void Main_Menu_Daemon::postprocess(queue<wstring>* errors) {
    if (cur_index == 0) {
        /*superdaemon -> subdaemon = new Select_Game_Menu_Daemon(daemon_id, layers,
                superdaemon -> keystrokes_to_pass_down,
                superdaemon -> errors_from_down_and_self, superdaemon);
        superdaemon -> subdaemon_name = "Select_Game_Menu_Daemon";*/
        superdaemon -> subdaemon = new Game_Daemon(daemon_id, layers,
                superdaemon -> keystrokes_to_pass_down,
                superdaemon -> errors_from_down_and_self, superdaemon);
        superdaemon -> subdaemon_name = "Game_Daemon";
    } else if (cur_index == 1) {
        superdaemon -> subdaemon = new Credits_Daemon(daemon_id, layers,
                superdaemon -> keystrokes_to_pass_down,
                superdaemon -> errors_from_down_and_self, superdaemon);
        superdaemon -> subdaemon_name = "Credits_Daemon";
    } else {
        errors -> push(L"!!!EXIT!!!");
    }
}

//what it does: Main_Menu_Daemon constructor
//inputs: daemon_id, pointer to vector of layers, pointer to queue of keystrokes,
//  pointer to queue of errors, pointer to superdaemon
//outputs: Main_Menu_Daemon
Main_Menu_Daemon::Main_Menu_Daemon(int daemon_id_in, vector<Layer>* layers,
        queue<int>* keystrokes_from_up, queue<wstring>* errors_to_pass_up, Daemon* superdaemon)
    : Menu_Daemon(daemon_id_in, layers, 1,
            "Main_Menu.scn", "Button",
            keystrokes_from_up, errors_to_pass_up, superdaemon) {
}

//the topmost daemon and the daemon that is always running, responsilbe for debug mode and 
//to display errors
class Info_Daemon : public Daemon {
private:
    bool display_debug;
    //what it does: overrided keystroke function to handle F5 
    //inputs: errors queue, key to process
    //outputs: true or false
    bool process_keystroke(queue<wstring>* errors, int key) {
        if (display_debug) {
            if ((*layers)[starting_layer + 1].check("keystroke")) {
                (*layers)[starting_layer + 1].remove(errors, "keystroke");
            }
            wstring wcs = L" ";
            wcs[0] = wchar_t(key);
            wstring out = to_wstring(key) + L": " + wcs;
            if (key < 10) {
                out += L"   ";
            } else if (key < 100) {
                out += L"  ";
            } else if (key < 256) {
                out += L" ";
            }
            (*layers)[starting_layer + 1].add(errors, "keystroke", *(new TGF_Object(errors, 0.5, 0.5, out)));
        }
        if (key == KEY_F(5)) {
            if (!display_debug) {
                TGF_Object grid(errors, 0, 0, L"");
                grid.bb_rows = LINES;
                grid.bb_cols = COLS;
                grid.alignx = grid.aligny = grid.offsetx = grid.offsety = 0;
                for (int i = 0; i < LINES; i++) {
                    for (int j = 0; j < COLS; j++) {
                        grid.graphics.push_back((i % 5 == 0 && j % 5 == 0 && i != LINES / 2) ? L'+' : L' ');
                        grid.opacity.push_back((i % 5 == 0 && j % 5 == 0 && i != LINES / 2));
                    }
                }
                (*layers)[starting_layer + 1].add(errors, "grid", grid);
            } else {
                if ((*layers)[starting_layer + 1].check("keystroke")) {
                    (*layers)[starting_layer + 1].remove(errors, "keystroke");
                }
                (*layers)[starting_layer + 1].remove(errors, "grid");
            }
            display_debug = !display_debug;
            /*if (subdaemon_name != "Keypress_Daemon") {
                terminate_subdaemon();
                subdaemon = new Keypress_Daemon(daemon_id + 1, layers, 1, 
                        keystrokes_to_pass_down,
                        errors_from_down_and_self);
                subdaemon_name = "Keypress_Daemon";
            }*/
            key_to_pass_down = key;
        } else {
            key_to_pass_down = key;
        }
        return true;
    }
    //what it does: overrided process error function, ends program if !!!EXIT!!! is received
    //inputs: errors queue, error to process
    //outputs: true or false
    bool process_error(queue<wstring>* errors, wstring error) {
        if (error.find(L"!!!END!!!") != string::npos) {
            delete subdaemon;
        }
        if (error.find(L"!!!EXIT!!!") != string::npos) {
            //wcerr << L"SDFGSDFG";
            errors -> push(L"!!!END!!!");
            return true;
        }
        wstring s = L"";
        if ((*layers)[starting_layer].check("ERRORS")) {
            s = (*layers)[starting_layer].objs.at("ERRORS").optional_string;
            (*layers)[starting_layer].remove(errors, "ERRORS");
        }
        s += error + L"\n";
        int count = 0;
        for (wchar_t c : s) count += (c == L'\n');
        if (count > 3) {
            while (s[0] != L'\n') s.erase(s.begin());
            s.erase(s.begin());
        }
        TGF_Object err(errors, 0, 0, COLS, s);
        err.optional_string = s;
        (*layers)[starting_layer].add(errors, "ERRORS", err);
        return false;
    }
public:
    //what it does: Info_Daemon constructor, also spawns Main_Menu_Daemon
    //inputs: daemon_id, pointer to vector of layers, number of layers, 
    //  pointer to queue of keystrokes, pointer to queue of errors
    //outputs: Info_Daemon
    Info_Daemon(int daemon_id_in, vector<Layer>* layers, int num_layers_in,
            queue<int>* keystrokes_from_up, queue<wstring>* errors_to_pass_up)
        : Daemon(daemon_id_in, layers, num_layers_in, keystrokes_from_up, errors_to_pass_up, nullptr) {
        subdaemon = new Main_Menu_Daemon(daemon_id + 1, layers,
                keystrokes_to_pass_down, errors_from_down_and_self, this);
        subdaemon_name = "Main_Menu_Daemon";
        preprocessed = true;
    }
};


vector<Layer>* layers;
thread* render_thread;
Daemon* info_daemon;

const int MILLISECONDS_PER_FRAME = 10;

//what it does: renders all the layers (starting from the topmost layer)
//inputs: the pointer to vector of layers
//outputs: void
void render(vector<Layer>* layers) {
    map<int, map<int, wchar_t>> prev_screen;
    while (!stop) {
        const auto now = chrono::steady_clock::now();
        set<pair<int, int>> s;
        for (int i = 0; i < LINES; i++) {
            for (int j = 0; j < COLS; j++) {
                if (!layers -> at(0).check(i, j)) s.insert(make_pair(i, j));
                else {
                    wchar_t temp = layers -> at(0).screen[i][j];
                    //wcerr << temp;
                    if (prev_screen.find(i) == prev_screen.end() || prev_screen[i].find(j) == prev_screen[i].end() || prev_screen[i][j] != temp) {
                        cchar_t ptr;
                        setcchar(&ptr, &temp, 0, 0, nullptr);
                        mvadd_wch(i, j, &ptr);
                        prev_screen[i][j] = temp;
                    }
                }
            }
        }
        try {
            for (int l = 1; l < layers -> size(); l++) {
                set<pair<int, int>> temp_s;
                for (auto p : s) {
                    if (layers -> at(l).check(p.first, p.second)) {
                        wchar_t temp = layers -> at(l).screen[p.first][p.second];
                        temp_s.insert(p);
                        if (prev_screen.find(p.first) == prev_screen.end() || prev_screen[p.first].find(p.second) == prev_screen[p.first].end() || prev_screen[p.first][p.second] != temp) {
                            cchar_t ptr;
                            setcchar(&ptr, &temp, 0, 0, nullptr);
                            if (nospace) {
                                mvadd_wch(p.first, p.second, &ptr);
                            }
                            temp_s.insert(p);
                            prev_screen[p.first][p.second] = temp;
                        }
                    }
                }
                for (auto p : temp_s) s.erase(p);
            }
        } catch (const std::out_of_range& e) {
            //cout << "Out of Range error.";
        }
        for (auto p : s) {
            wchar_t temp = L' ';
            if (prev_screen.find(p.first) == prev_screen.end() || prev_screen[p.first].find(p.second) == prev_screen[p.first].end() || prev_screen[p.first][p.second] != temp) {
                cchar_t ptr;
                setcchar(&ptr, &temp, 0, 0, nullptr);
                mvadd_wch(p.first, p.second, &ptr);
                prev_screen[p.first][p.second] = temp;
            }
        }
        this_thread::sleep_until(now + *(new chrono::duration<int, std::milli>(MILLISECONDS_PER_FRAME)));
    }
    //wcerr << L"stoepd" << endl;
}

//what it does: handles interrupt signal (does not rly work)
//inputs: signal
//outputs: void
void handle_int(int sig) {
    signal(SIGINT, SIG_IGN);
    //doesnt work :(
    clear();
    mvaddstr(LINES / 2, COLS / 2 - 5, "EXITING...");
    //this_thread::sleep_for(chrono::milliseconds(500));
    endwin();
    exit(0);
}

//what it does: initialize ncurses
void init() {
    initscr();
    noecho();
    cbreak();
    nonl();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

}

//what it does: handles window change signal
//inputs: signal
//outputs: void
void handle_winch(int sig) {
    signal(SIGWINCH, SIG_IGN);
    stop = true;
    endwin();
    /*init();
    clear();
    printw("press enter when you are done resizing.");
    nodelay(stdscr, FALSE);
    getch();
    nodelay(stdscr, TRUE);*/
    cout << "press enter when you are done resizing.";
    string s;
    getline(cin, s);
    init();
    winsize ws;
    ioctl(0, TIOCGWINSZ, &ws);
    resizeterm(ws.ws_row, ws.ws_col);
    //wcerr << LINES << L" " << COLS << endl;
    //wcerr << LINES << L" " << COLS << endl;
    info_daemon -> redraw(LINES, COLS);
    stop = false;
    render_thread = new thread(render, layers);
    signal(SIGWINCH, handle_winch);
}

int main() {
    nospace = true;
    setlocale(LC_CTYPE, "");
    init();
    signal(SIGWINCH, handle_winch);
    signal(SIGINT, handle_int);
    queue<int>* keypress = new queue<int>;
    queue<wstring>* info_daemon_errors = new queue<wstring>;
    layers = new vector<Layer>;
    info_daemon = new Info_Daemon(1, layers, 2, keypress, info_daemon_errors);
    render_thread = new thread(render, layers);
    while (true) {
        int temp = ERR;
        if (!running_game) temp = getch();
        if (temp != ERR) keypress -> push(temp);
        if (!info_daemon_errors -> empty()) {
            stop = true;
            if (info_daemon_errors -> size() >= 1 && info_daemon_errors -> front() == L"!!!END!!!") {
                break;
            }
            mvaddwstr(0, 0, s2ws("ERROR ERROR:").c_str());
            int r = 1;
            while (!info_daemon_errors -> empty()) {
                //mvaddwstr(r, 0, info_daemon_errors -> front().c_str());
                //wcerr << info_daemon_errors -> front() << endl;
                r++;
                info_daemon_errors -> pop();
            }
            this_thread::sleep_for(chrono::milliseconds(1000));
            break;
        }
        //this_thread::sleep_for(chrono::milliseconds(1));
    }
    delete info_daemon;
    delete layers;
    delete keypress;
    delete info_daemon_errors;
    clear();
    endwin();
}
