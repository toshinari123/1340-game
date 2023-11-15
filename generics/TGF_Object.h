#ifndef TGF_OBJECT_H
#define TGF_OBJECT_H

#include <bits/stdc++.h>
using namespace std;

class TGF_Object {
private:
    void check_row(queue<string>* errors, string s, string p, int row) {
        if (bb_cols == 0) bb_cols = s.size();
        if (bb_cols != s.size()) {
            errors -> push("TGF_Object_check_row: row length mismatch in row " + to_string(row));
            return;
        }
        for (char c : s) {
            int s = 0;
            if (p == "*") s = 1;
            else for (char d : p) s += (c == d);
            if (s == 0) {
                errors -> push("TGF_Object_check_row: character is not one of " + p + "in row " + to_string(row));
            }
        }
    }
public:
    int bb_rows, bb_cols;
    int pp_row, pp_col;
    double alignx, aligny;
    int offsetx, offsety;
    int br_offsetx, br_offsety;
    int *graphics, *opacity, *foreground, *background;
    bool is_braille;
    string optional_string;
    TGF_Object(queue<string>* errors, string p, double ax, double ay, int ox, int oy) {
        bb_cols = bb_rows = pp_row = pp_col = alignx = aligny = offsetx = offsety = br_offsetx = br_offsety = 0;
        ifstream f;
        f.open(p);
        if (f.bad()) {
            errors -> push("TGF_Object_file_constructor: invalid TGF_Object file path " + p);
            return;
        }
        int cur_row = 0;
        string s;
        getline(f, s);
        cur_row++;
        if (s != "[position_point]") {
            errors -> push("TGF_Object_file_constructor: first row is not [position_point] in " + p);
            return;
        }
        int count = 0;
        while (getline(f, s)) {
            this -> check_row(errors, s, "01", cur_row);
            cur_row++;
            for (int j = 0; j < s.size(); j++) {
                if (s[j] == '1') {
                    count += 1, pp_row = bb_rows, pp_col = j;
                }
            }
            bb_rows++;
        }
        for (int i = 0; i < bb_rows; i++) {
            for (int j = 0; j < bb_cols; j++) {
                graphics[i * bb_rows + j] = opacity[i * bb_rows + j] = background[i * bb_rows + j] = foreground[i * bb_rows + j] = 0;
            }
        }
        if (count == 0) {
            errors -> push("TGF_Object_file_constructor: no position point specified");
        }
        if (count > 1) {
            errors -> push("TGF_Object_file_constructor: multiple position points specified");
        }
        delete graphics;
        delete opacity;
        delete foreground;
        delete background;
        graphics = new int[bb_rows * bb_cols];
        opacity = new int[bb_rows * bb_cols];
        foreground = new int[bb_rows * bb_cols];
        background = new int[bb_rows * bb_cols];
        while (true) {
            string p = "01";
            if (s[0] != '[' || s[s.size() - 1] != ']') {
                errors -> push("TGF_Object_file_constructor: section heading format wrong in row " + to_string(cur_row) + " of " + p);
            }
            cur_row++;
            if (s == "[graphics]") p = "*";
            if (s == "[foreground_color]" || s == "[background_color]") p = "BWrgbcym";
            string arr[bb_rows];
            for (int i = 0; i < bb_rows; i++) {
                cur_row++;
                if (!getline(f, arr[i])) {
                    errors -> push("TGF_Object_file_constructor: missing line " + to_string(i) + " of section " + s + " in file " + p);
                    break;
                }
                this -> check_row(errors, arr[i], p, cur_row);
            }
            if (s == "[graphics]") {
                for (int i = 0; i < bb_rows; i++) {
                    for (int j = 0; j < bb_cols; j++) {
                        graphics[i * bb_rows + j] = arr[i][j];
                    }
                }
            } else if (s ==  "[opacity]") {
                for (int i = 0; i < bb_rows; i++) {
                    for (int j = 0; j < bb_cols; j++) {
                        opacity[i * bb_rows + j] = (arr[i][j] == '1');
                    }
                }
            } else if (s ==  "[foreground_color]") {

            } else if (s ==  "[background_color]") {

            } else if (s ==  "[foreground_intensity]") {

            } else if (s ==  "[background_intensity]") {

            }
            //work on colors later
            //https://www.linuxjournal.com/content/about-ncurses-colors-0
            if (!getline(f, s)) break;
        }
        alignx = ax;
        aligny = ay;
        offsetx = ox;
        offsety = oy;
    }
    //TODO: make constructor for braille
    //mvprintw no wrap
    TGF_Object(queue<string>* errors, int x, int y, string s, ...) {
        va_list vl;
        va_start(vl, s);
        string line;
        char* out;
        vsprintf(out, s.c_str(), vl);
        optional_string = out;
        is_braille = false;
        alignx = aligny = 0;
        offsetx = x;
        offsety = y;
        stringstream ss(s);
        bb_rows = bb_cols = pp_row = pp_col = 0;
        while (getline(ss, line)) {
            bb_rows++;
            bb_cols = max(bb_cols, int(line.size()));
        }
        delete graphics;
        delete opacity;
        delete foreground;
        delete background;
        graphics = new int[bb_rows * bb_cols];
        opacity = new int[bb_rows * bb_cols];
        foreground = new int[bb_rows * bb_cols];
        background = new int[bb_rows * bb_cols];
        int row = 0;
        stringstream ss2(s);
        while (getline(ss2, line)) {
            for (int j = 0; j < line.size(); j++) {
                graphics[row * bb_cols + j] = line[j];
                opacity[row * bb_cols + j] = 1;
            }
        }
        va_end(vl);
    }
    //mvprintw with align coordinates (fractional)
    TGF_Object(queue<string>* errors, double x, double y, string s, ...) {
        va_list vl;
        va_start(vl, s);
        string line;
        char* out;
        vsprintf(out, s.c_str(), vl);
        optional_string = out;
        is_braille = false;
        alignx = x;
        aligny = y;
        offsetx = offsety = 0;
        stringstream ss(s);
        bb_rows = bb_cols = pp_row = pp_col = 0;
        while (getline(ss, line)) {
            bb_rows++;
            bb_cols = max(bb_cols, int(line.size()));
        }
        delete graphics;
        delete opacity;
        delete foreground;
        delete background;
        graphics = new int[bb_rows * bb_cols];
        opacity = new int[bb_rows * bb_cols];
        foreground = new int[bb_rows * bb_cols];
        background = new int[bb_rows * bb_cols];
        int row = 0;
        stringstream ss2(s);
        while (getline(ss2, line)) {
            for (int j = 0; j < line.size(); j++) {
                graphics[row * bb_cols + j] = line[j];
                opacity[row * bb_cols + j] = 1;
            }
        }
        va_end(vl);
    }
    //mvprintw wrap
    TGF_Object(queue<string>* errors, int x, int y, int width, string s, ...) {
        va_list vl;
        va_start(vl, s);
        string line;
        char* out;
        vsprintf(out, s.c_str(), vl);
        optional_string = out;
        is_braille = false;
        alignx = aligny = 0;
        offsetx = x;
        offsety = y;
        stringstream ss(s);
        bb_rows = pp_row = pp_col = 0;
        bb_cols = width;
        while (getline(ss, line)) {
            bb_rows += line.size() / width + int(line.size() % width > 0);
        }
        delete graphics;
        delete opacity;
        delete foreground;
        delete background;
        graphics = new int[bb_rows * bb_cols];
        opacity = new int[bb_rows * bb_cols];
        for (int i = 0; i < bb_rows; i++) {
            for (int j = 0; j < bb_cols; j++) {
                opacity[i * bb_cols+ j] = 1;
            }
        }
        foreground = new int[bb_rows * bb_cols];
        background = new int[bb_rows * bb_cols];
        int row = 0;
        stringstream ss2(s);
        while (getline(ss2, line)) {
            for (int i = 0; i < line.size() / width; i++) {
                for (int j = 0; j < (i == line.size() / width - 1 ? line.size() % width : width); j++) {
                    graphics[row * bb_cols + j] = line[j];
                }
                row++;
            }
        }
        va_end(vl);
    }
    ~TGF_Object() {
        delete graphics;
        delete opacity;
        delete foreground;
        delete background;
    }
    void move_diff(int dx, int dy) {
        offsetx += dx;
        offsety += dy;
    }
    void move_abs(int x, int y) {
        alignx = aligny = 0;
        offsetx = x;
        offsety = y;
    }
    void set_align(double ax, double ay) {
        alignx = ax;
        aligny = ay;
        offsetx = offsety = 0;
    }
    void set_all(double ax, double ay, int ox, int oy) {
        alignx = ax;
        aligny = ay;
        offsetx = ox;
        offsety = oy;
    }
};

#endif
