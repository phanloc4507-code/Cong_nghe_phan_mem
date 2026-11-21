#include "Category.h"
#include "TextUtil.h"
#include <fstream>

bool CategoryManager::load(const std::string& path) {
    items_.clear();
    std::ifstream in(path);
    if (!in) return false;
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;
        auto f = split(line, '|');
        if (f.size() < 2) continue;
        try {
            items_.push_back({std::stoi(f[0]), f[1]});
        } catch (...) { /* bo qua dong loi */ }
    }
    return true;
}

bool CategoryManager::save(const std::string& path) const {
    std::ofstream out(path);
    if (!out) return false;
    for (const auto& c : items_) out << c.id << '|' << c.name << '\n';
    return true;
}

const Category* CategoryManager::findById(int id) const {
    for (const auto& c : items_) if (c.id == id) return &c;
    return nullptr;
}

std::string CategoryManager::nameOf(int id) const {
    const Category* c = findById(id);
    return c ? c->name : "(khong ro)";
}

bool CategoryManager::nameExists(const std::string& name, int exceptId) const {
    std::string low = toLower(name);
    for (const auto& c : items_)
        if (c.id != exceptId && toLower(c.name) == low) return true;
    return false;
}

bool CategoryManager::add(const std::string& rawName, std::string& err) {
    std::string name = sanitize(rawName);
    if (name.empty()) { err = "Ten loai xe khong duoc de trong."; return false; }
    if (nameExists(name, -1)) { err = "Loai xe nay da ton tai."; return false; }
    int maxId = 0;
    for (const auto& c : items_) maxId = std::max(maxId, c.id);
    items_.push_back({maxId + 1, name});
    return true;
}

bool CategoryManager::rename(int id, const std::string& rawName, std::string& err) {
    std::string name = sanitize(rawName);
    if (name.empty()) { err = "Ten loai xe khong duoc de trong."; return false; }
    for (auto& c : items_) {
        if (c.id != id) continue;
        if (nameExists(name, id)) { err = "Ten loai xe bi trung."; return false; }
        c.name = name;
        return true;
    }
    err = "Khong tim thay loai xe.";
    return false;
}

bool CategoryManager::remove(int id) {
    for (auto it = items_.begin(); it != items_.end(); ++it)
        if (it->id == id) { items_.erase(it); return true; }
    return false;
}
