#include <bits/stdc++.h>
using namespace std;

class TGF_Object {
private:
    void check_row(string s, string p, queue<string>* errors, int row) {
        if (this -> bb_cols == 0) bb_cols = s.size();
        if (this -> bb_cols != s.size()) {
            errors.push("TGF_Object_check_row: row length mismatch in row " + to_string(row));
            return;
        }
        for (char c : s) {
            int s = 0;
            if (p == "*") s = 1;
            else for (char d : p) s += (c == d);
            if (s == 0) {
                errors.push("TGF_Object_check_row: character is not one of " + p + "in row " + to_string(row));
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
    TGF_Object(string p, double ax, double ay, int ox, int oy, queue<string>* errors) {
        bb_cols = bb_rows = pp_row = pp_col = alignx = aligny = offsetx = offsety = br_offsetx = br_offsety = 0;
        ifstream f;
        f.open(p);
        if (f.bad()) {
            errors.push("TGF_Object_file_constructor: invalid TGF_Object file path " + p);
            return;
        }
        int cur_row = 0;
        string s;
        getline(f, s);
        cur_row++;
        if (s != "[position_point]") {
            errors.push("TGF_Object_file_constructor: first row is not [position_point] in " + p);
            return;
        }
        int count = 0;
        while (getline(f, s)) {
            this -> check_row(s, "01", errors, cur_row);
            cur_row++;
            catch (string s) {
                throw "TGF_Init_Exception: " + s;
            }
            for (int j = 0; j < s.size(); j++) {
                if (c == '1') {
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
            errors.push("TGF_Object_file_constructor: no position point specified");
        }
        if (count > 1) {
            errors.push("TGF_Object_file_constructor: multiple position points specified");
        }
        while (true) {
            string p = "01";
            if (s[0] != '[' || s[s.size() - 1] != ']') {
                errors.push("TGF_Object_file_constructor: section heading format wrong in row " + to_string(cur_row) + " of " + p);
            }
            cur_row++;
            if (s == "[graphics]") p = "*";
            if (s == "[foreground_color]" || s == "[background_color]") p = "BWrgbcym";
            string arr[bb_rows];
            for (int i = 0; i < bb_rows; i++) {
                cur_row++;
                if (!getline(f, arr[i])) {
                    errors.push("TGF_Object_file_constructor: missing line " + to_string(i) + " of section " + s + " in file " + p);
                    break;
                }
                this -> check_row(arr[i], p, errors, cur_row);
            }
            graphics = new int[bb_rows * bb_cols];
            opacity = new int[bb_rows * bb_cols];
            foreground = new int[bb_rows * bb_cols];
            background = new int[bb_rows * bb_cols];
            switch (s) {
                case "[graphics]":
                    for (int i = 0; i < bb_rows; i++) {
                        for (int j = 0; j < bb_cols; j++) {
                            graphics[i * bb_rows + j] = arr[i][j];
                        }
                    }
                    break;
                case "[opacity]":
                    for (int i = 0; i < bb_rows; i++) {
                        for (int j = 0; j < bb_cols; j++) {
                            opacity[i * bb_rows + j] = (arr[i][j] == '1');
                        }
                    }
                    break;
                //work on colors later
                //https://www.linuxjournal.com/content/about-ncurses-colors-0
                case "[foreground_color]":
                    break;
                case "[background_color]":
                    break;
                case "[foreground_intensity]":
                    break;
                case "[background_intensity]":   
                    break;
            }
            if (getline(f, s) == -1) break;
        }
        alignx = ax;
        aligny = ay;
        offsetx = ox;
        offsety = oy;
    }
    //TODO: make constructor for braille
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
}
