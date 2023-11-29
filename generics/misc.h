#ifndef MISC_GAME_FUNCTIONS_H
#define MISC_GAME_FUNCTIONS_H

#include <codecvt>
#include <locale>
using namespace std;

extern bool stop;

wstring s2ws(const string& str);

string ws2s(const wstring& wstr);

#endif
