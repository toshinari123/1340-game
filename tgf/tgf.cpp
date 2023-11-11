#include <bits/stdc++.h>
using namespace std;

class TGF_Object {
private:
    void check_row(string s, string p) {
        if (this -> bb_cols == 0) this -> bb_cols = s.size();
        if (this -> bb_cols != s.size()) throw "row length mismatch";
        for (char c : s) {
            int s = 0;
            if (p == "*") s = 1;
            else for (char d : p) s += (c == d);
            if (s == 0) throw "invalid character";
        }
    }
public:
    int bb_rows, bb_cols;
    int pp_row, pp_col;
    int pp_posx, pp_posy;
    int *graphics, *opacity, *foreground, *background;
    TGF_Object(string, p, x, y) {
        ifstream f;
        f.open(p);
        string s;
        getline(f, s);
        if (s != "[position_point]") throw "TGF_Init_Exception: first section is not position_point";
        this -> bb_cols = 0;
        this -> bb_rows = 0;
        int count = 0;
        while (getline(f, s)) {
            try {
                this -> check_row(s, "01");
            }
            catch (string s) {
                throw "TGF_Init_Exception: " + s;
            }
            for (int j = 0; j < bb_cols; j++) if (c == '1') {
                count += 1, pp_row = bb_rows, pp_col = j;
            }
            this -> bb_rows++;
        }
        if (count == 0) throw "TGF_Init_Exception: no position point specified";
        if (count > 1) throw "TGF_Init_Exception: multiple position points specified";
        while (true) {
            string p = "01";
            if (s[0] != '[' || s[s.size() - 1] != ']') throw "TGF_Init_Exception: section heading format wrong";
            if (s == "[graphics]") p = "*";
            if (s == "[foreground_color]" || s == "[background_color]") p = "BWrgbcym";
            string arr[bb_rows];
            try {
                for (int i = 0; i < bb_rows; i++) {
                    getline(f, arr[i]);
                    this -> check_row(arr[i], p);
                }
            }
            catch (string s) {
                throw "TGF_Init_Exception: " + s;
            }
            graphics = new char[bb_rows * bb_cols];
            opacity = new bool[bb_rows * bb_cols];
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
        pp_posx = x;
        pp_posy = y;
    }
}
