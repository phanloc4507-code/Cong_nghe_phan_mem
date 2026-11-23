// FileManager.cpp
#include "FileManager.h"
#include <fstream>

std::vector<std::string> FileManager::readLines(const std::string& filePath) {
    std::vector<std::string> lines;
    std::ifstream file(filePath);
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) lines.push_back(line);
    }
    return lines;
}

bool FileManager::writeLines(const std::string& filePath, const std::vector<std::string>& lines) {
    std::ofstream file(filePath);
    if (!file.is_open()) return false;
    for (const auto& line : lines) {
        file << line << "\n";
    }
    return true;
}