#ifndef TGF_OBJECT_H
#define TGF_OBJECT_H

#include <bits/stdc++.h>
#include "misc.h"
using namespace std;

class TGF_Object {
private:
    void check_row(queue<string>* errors, wstring s, string p, int row, string path) {
        if (bb_cols == 0) bb_cols = s.size();
        if (bb_cols != s.size()) {
            errors -> push("TGF_Object_check_row: row length mismatch in row " + to_string(row) + " in file " + path);
            return;
        }
        for (char c : s) {
            int s = 0;
            if (p == "*") s = 1;
            else for (char d : p) s += (c == d);
            if (s == 0) {
                errors -> push("TGF_Object_check_row: character is not one of " + p + "in row " + to_string(row) + " of file " + path);
            }
        }
    }
public:
    int bb_rows, bb_cols;
    int pp_row, pp_col;
    double alignx, aligny;
    int offsetx, offsety;
    int br_offsetx, br_offsety;
    vector<int> opacity, foreground, background;
    vector<wchar_t> graphics;
    bool is_braille;
    string optional_string;
    TGF_Object(queue<string>* errors, string path, double ax, double ay, int ox, int oy) {
        bb_cols = bb_rows = pp_row = pp_col = alignx = aligny = offsetx = offsety = br_offsetx = br_offsety = 0;
        wifstream f;
        f.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
        f.open(path);
        if (f.bad()) {
            errors -> push("TGF_Object_file_constructor: invalid TGF_Object file path " + path);
            return;
        }
        int cur_row = 0;
        wstring s;
        getline(f, s);
        if (s[s.size() - 1] == '\r') s.pop_back();
        cur_row++;
        if (ws2s(s) != "[position_point]") {
            errors -> push("TGF_Object_file_constructor: first row is not [position_point] in " + path);
            return;
        }
        int count = 0;
        while (getline(f, s)) {
            if (s[s.size() - 1] == '\r') s.pop_back();
            if (s.empty()) break;
            this -> check_row(errors, s, "01", cur_row, path);
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
                graphics.push_back(0);
                opacity.push_back(0);
                background.push_back(0);
                foreground.push_back(0);
            }
        }
        if (count == 0) {
            errors -> push("TGF_Object_file_constructor: no position point specified in " + path);
        }
        if (count > 1) {
            errors -> push("TGF_Object_file_constructor: multiple position points specified in " + path);
        }
        while (true) {
            getline(f, s);
            if (s.empty()) break;
            if (s[s.size() - 1] == '\r') s.pop_back();
            string p = "01";
            if (s[0] != '[' || s[s.size() - 1] != ']') {
                errors -> push("TGF_Object_file_constructor: section heading format wrong in row " + to_string(cur_row) + " of " + path);
            }
            cur_row++;
            if (ws2s(s) == "[graphics]") p = "*";
            if (ws2s(s) == "[foreground_color]" || ws2s(s) == "[background_color]") p = "BWrgbcym";
            wstring arr[bb_rows];
            for (int i = 0; i < bb_rows; i++) {
                cur_row++;
                getline(f, arr[i], L'\n');
                wcerr << arr[i].size() << ": " << arr[i] << endl;
                if (arr[i].empty()) {
                    errors -> push("TGF_Object_file_constructor: missing line " + to_string(i) + " of section " + ws2s(s) + " in file " + path);
                    break;
                }
                if (arr[i][arr[i].size() - 1] == '\r') arr[i].pop_back();
                this -> check_row(errors, arr[i], p, cur_row, path);
            }
            if (ws2s(s) == "[graphics]") {
                for (int i = 0; i < bb_rows; i++) {
                    for (int j = 0; j < bb_cols; j++) {
                        graphics[i * bb_cols + j] = arr[i][j];
                    }
                }
            } else if (ws2s(s) ==  "[opacity]") {
                for (int i = 0; i < bb_rows; i++) {
                    for (int j = 0; j < bb_cols; j++) {
                        opacity[i * bb_cols + j] = (arr[i][j] == '1');
                    }
                }
            } else if (ws2s(s) == "[foreground_color]") {

            } else if (ws2s(s) == "[background_color]") {

            } else if (ws2s(s) == "[foreground_intensity]") {

            } else if (ws2s(s) == "[background_intensity]") {

            }
            //work on colors later
            //https://www.linuxjournal.com/content/about-ncurses-colors-0
            if (!getline(f, s)) break;
            if (s[s.size() - 1] == '\r') s.pop_back();
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
        char out[10000];
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
        for (int i = 0; i < bb_rows; i++) {
            for (int j = 0; j < bb_cols; j++) {
                graphics.push_back(0);
                opacity.push_back(0);
                background.push_back(0);
                foreground.push_back(0);
            }
        }
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
        char out[10000];
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
        for (int i = 0; i < bb_rows; i++) {
            for (int j = 0; j < bb_cols; j++) {
                graphics.push_back(0);
                opacity.push_back(0);
                background.push_back(0);
                foreground.push_back(0);
            }
        }
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
        char out[10000];
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
        for (int i = 0; i < bb_rows; i++) {
            for (int j = 0; j < bb_cols; j++) {
                graphics.push_back(0);
                opacity.push_back(1);
                background.push_back(0);
                foreground.push_back(0);
            }
        }
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
    TGF_Object& operator=(const TGF_Object& o) {
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
