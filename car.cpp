#include "Car.h"
#include "TextUtil.h"
# include <bits/stdc++.h>

std::string statusToString(CarStatus s) {
    switch (s) {
        case CarStatus::Ready:       return "Ready";
        case CarStatus::Rented:      return "Rented";
        case CarStatus::Maintenance: return "Maintenance";
    }
    return "Ready";
}

bool statusFromString(const std::string& text, CarStatus& out) {
    std::string t = toLower(trim(text));
    if (t == "ready")       { out = CarStatus::Ready; return true; }
    if (t == "rented")      { out = CarStatus::Rented; return true; }
    if (t == "maintenance") { out = CarStatus::Maintenance; return true; }
    return false;
}

// ---------------- Car ----------------
Car::Car(std::string id, std::string name, std::string description, int seats,
         std::string transmission, std::string fuel, long long pricePerDay,
         int discountPercent, CarStatus status, int categoryId)
    : id_(std::move(id)), name_(std::move(name)), description_(std::move(description)),
      transmission_(std::move(transmission)), fuel_(std::move(fuel)), seats_(seats),
      pricePerDay_(pricePerDay), discountPercent_(discountPercent), status_(status),
      categoryId_(categoryId) {}

long long Car::finalPrice() const {
    return pricePerDay_ * (100 - discountPercent_) / 100;
}

std::string Car::toLine() const {
    return id_ + '|' + name_ + '|' + description_ + '|' + std::to_string(seats_) + '|' +
           transmission_ + '|' + fuel_ + '|' + std::to_string(pricePerDay_) + '|' +
           std::to_string(discountPercent_) + '|' + statusToString(status_) + '|' +
           std::to_string(rentCount_) + '|' + std::to_string(viewCount_) + '|' +
           std::to_string(categoryId_);
}

bool Car::fromLine(const std::string& line, Car& out) {
    auto f = split(line, '|');
    if (f.size() < 12) return false;
    try {
        CarStatus st;
        if (!statusFromString(f[8], st)) return false;
        out = Car(f[0], f[1], f[2], std::stoi(f[3]), f[4], f[5], std::stoll(f[6]),
                  std::stoi(f[7]), st, std::stoi(f[11]));
        out.setCounters(std::stoi(f[9]), std::stoi(f[10]));
    } catch (...) {
        return false;
    }
    return true;
}

std::string Car::validate() const {
    if (trim(id_).empty()) return "Ma xe khong duoc de trong.";
    if (trim(name_).empty()) return "Ten xe khong duoc de trong.";
    if (seats_ <= 0) return "So cho ngoi phai lon hon 0.";
    if (pricePerDay_ <= 0) return "Gia thue/ngay phai lon hon 0.";
    if (discountPercent_ < 0 || discountPercent_ > 100) return "Giam gia phai tu 0 den 100%.";
    return "";
}

// ---------------- CarManager ----------------
bool CarManager::load(const std::string& path) {
    cars_.clear();
    std::ifstream in(path);
    if (!in) return false;
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;
        Car c;
        if (Car::fromLine(line, c)) cars_.push_back(c);   // dong loi thi bo qua
    }
    return true;
}

bool CarManager::save(const std::string& path) const {
    std::ofstream out(path);
    if (!out) return false;
    for (const auto& c : cars_) out << c.toLine() << '\n';
    return true;
}

const Car* CarManager::findById(const std::string& id) const {
    std::string low = toLower(id);
    for (const auto& c : cars_) if (toLower(c.id()) == low) return &c;
    return nullptr;
}

Car* CarManager::findById(const std::string& id) {
    return const_cast<Car*>(static_cast<const CarManager*>(this)->findById(id));
}

std::vector<const Car*> CarManager::listAll() const {
    std::vector<const Car*> v;
    for (const auto& c : cars_) v.push_back(&c);
    return v;
}

std::vector<const Car*> CarManager::byCategory(int categoryId) const {
    std::vector<const Car*> v;
    for (const auto& c : cars_) if (c.categoryId() == categoryId) v.push_back(&c);
    return v;
}

std::vector<const Car*> CarManager::search(const std::string& keyword) const {
    std::vector<const Car*> v;
    std::string key = toLower(trim(keyword));
    if (key.empty()) return v;
    for (const auto& c : cars_)
        if (toLower(c.name()).find(key) != std::string::npos) v.push_back(&c);
    return v;
}

void CarManager::sortCars(std::vector<const Car*>& v, SortKey key, bool ascending) {
    if (key == SortKey::None) return;
    auto value = [key](const Car* c) -> long long {
        switch (key) {
            case SortKey::Price:     return c->pricePerDay();
            case SortKey::Discount:  return c->discountPercent();
            case SortKey::RentCount: return c->rentCount();
            default:                 return 0;
        }
    };
    std::stable_sort(v.begin(), v.end(), [&](const Car* a, const Car* b) {
        return ascending ? value(a) < value(b) : value(a) > value(b);
    });
}

bool CarManager::add(const Car& car, std::string& err) {
    err = car.validate();
    if (!err.empty()) return false;
    if (findById(car.id())) { err = "Ma xe da ton tai."; return false; }
    cars_.push_back(car);
    return true;
}

bool CarManager::update(const Car& updated, std::string& err) {
    err = updated.validate();
    if (!err.empty()) return false;
    Car* cur = findById(updated.id());
    if (!cur) { err = "Khong tim thay xe."; return false; }
    int rent = cur->rentCount(), view = cur->viewCount();
    *cur = updated;
    cur->setCounters(rent, view);      // giu nguyen thong ke
    return true;
}

bool CarManager::remove(const std::string& id) {
    std::string low = toLower(id);
    for (auto it = cars_.begin(); it != cars_.end(); ++it)
        if (toLower(it->id()) == low) { cars_.erase(it); return true; }
    return false;
}

bool CarManager::updateStatus(const std::string& id, CarStatus status, std::string& err) {
    Car* c = findById(id);
    if (!c) { err = "Khong tim thay xe."; return false; }
    c->setStatus(status);
    return true;
}

std::string CarManager::generateId() const {
    int maxNum = 0;
    for (const auto& c : cars_) {
        const std::string& id = c.id();
        if (id.size() > 1 && (id[0] == 'C' || id[0] == 'c')) {
            try { maxNum = std::max(maxNum, std::stoi(id.substr(1))); } catch (...) {}
        }
    }
    std::string n = std::to_string(maxNum + 1);
    while (n.size() < 3) n = "0" + n;
    return "C" + n;
}

void CarManager::viewDetail(const std::string& id) {
    if (Car* c = findById(id)) c->increaseViewCount();
}
