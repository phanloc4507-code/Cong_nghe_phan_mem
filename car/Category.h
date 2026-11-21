#pragma once
#include <string>
#include <vector>

struct Category {
    int id = 0;
    std::string name;
};

class CategoryManager {
public:
    // File: moi dong "id|ten loai"
    bool load(const std::string& path);
    bool save(const std::string& path) const;

    const std::vector<Category>& all() const { return items_; }
    const Category* findById(int id) const;
    std::string nameOf(int id) const;            // "(khong ro)" neu khong tim thay

    bool add(const std::string& name, std::string& err);
    bool rename(int id, const std::string& newName, std::string& err);
    // Nguoi goi phai kiem tra loai xe con xe dang dung hay khong truoc khi xoa.
    bool remove(int id);

private:
    bool nameExists(const std::string& name, int exceptId) const;
    std::vector<Category> items_;
};
