#ifndef TGF_OBJECT_H
#define TGF_OBJECT_H

#include <bits/stdc++.h>
#include "misc.h"
using namespace std;

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
    void load_file(queue<wstring>* errors, string path) {
        bb_cols = bb_rows = pp_row = pp_col = 0;
        wifstream f;
        f.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
        f.open(path);
        if (f.bad()) {
            errors -> push(L"TGF_Object_file_constructor: invalid TGF_Object file path " + s2ws(path));
            return;
        }
        int cur_row = 0;
        wstring s;
        getline(f, s);
        if (s[s.size() - 1] == '\r') s.pop_back();
        cur_row++;
        if (s != L"[position_point]") {
            errors -> push(L"TGF_Object_file_constructor: first row is not [position_point] in " + s2ws(path));
            return;
        }
        int count = 0;
        while (getline(f, s)) {
            if (s[s.size() - 1] == '\r') s.pop_back();
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
                wcerr << arr[i].size() << ": " << arr[i] << endl;
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
        wcerr << L"loaded: " << pp_row << L", " << pp_col << L"\n";
    }
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
    //TODO: make constructor for braille
    //mvprintw no wrap
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
    //mvprintw with align coordinates (fractional)
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
    //mvprintw wrap
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
            wcerr << L"whyy " << line << endl;
            for (int i = 0; i <= line.size() / width; i++) {
                for (int j = 0; j < ((i == (line.size() / width)) ? line.size() % width : width); j++) {
                    graphics[row * width + j] = line[i * width + j];
                }
                row++;
            }
        }
        for (int i = 0; i < bb_rows; i++) {
            for (int j = 0; j < bb_cols; j++) {
                wcerr << graphics[i * bb_cols + j];
            }
            wcerr << endl;
        }
        wcerr << L"eh\n";
        va_end(vl);
    }
    ~TGF_Object() {
       opacity.clear();
       foreground.clear();
       background.clear();
       graphics.clear();
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
