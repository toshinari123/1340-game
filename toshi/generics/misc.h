#ifndef MISC_H
#define MISC_H

#include <bits/stdc++.h>
using namespace std;

wstring s2ws(const string& str) {
    using convert_typeX = codecvt_utf8<wchar_t>;
    wstring_convert<convert_typeX, wchar_t> converterX;
    return converterX.from_bytes(str);
}

string ws2s(const wstring& wstr) {
    using convert_typeX = codecvt_utf8<wchar_t>;
    wstring_convert<convert_typeX, wchar_t> converterX;
    return converterX.to_bytes(wstr);
}

#endif
