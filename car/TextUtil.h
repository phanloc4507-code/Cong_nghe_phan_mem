#pragma once
// Ham xu ly chuoi dung chung cho phan Car / Category (header-only).
// Neu Nguoi 1 da co Utils tuong tu thi co the doi sang dung Utils.
#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

inline std::string trim(const std::string& s) {
    size_t a = 0, b = s.size();
    while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;
    return s.substr(a, b - a);
}

inline std::string toLower(std::string s) {
    for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

// Tach chuoi theo ky tu phan cach, giu ca phan tu rong.
inline std::vector<std::string> split(const std::string& s, char sep) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        if (c == sep) { out.push_back(cur); cur.clear(); }
        else cur += c;
    }
    out.push_back(cur);
    return out;
}

// Ky tu '|' dung de phan cach cot trong file, nen doi thanh '/' khi nguoi dung nhap.
inline std::string sanitize(std::string s) {
    std::replace(s.begin(), s.end(), '|', '/');
    return trim(s);
}
