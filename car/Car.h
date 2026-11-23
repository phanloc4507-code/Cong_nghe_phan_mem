#pragma once
#include <string>
#include <vector>

enum class CarStatus { Ready, Rented, Maintenance };
std::string statusToString(CarStatus s);                      // "Ready" / "Rented" / "Maintenance"
bool statusFromString(const std::string& text, CarStatus& out);

class Car {
public:
    Car() = default;
    Car(std::string id, std::string name, std::string description, int seats,
        std::string transmission, std::string fuel, long long pricePerDay,
        int discountPercent, CarStatus status, int categoryId);

    // --- getters ---
    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& description() const { return description_; }
    int seats() const { return seats_; }
    const std::string& transmission() const { return transmission_; }
    const std::string& fuel() const { return fuel_; }
    long long pricePerDay() const { return pricePerDay_; }
    int discountPercent() const { return discountPercent_; }
    CarStatus status() const { return status_; }
    int rentCount() const { return rentCount_; }
    int viewCount() const { return viewCount_; }
    int categoryId() const { return categoryId_; }

    long long finalPrice() const;            // gia/ngay sau khi giam
    bool isAvailable() const { return status_ == CarStatus::Ready; }

    // --- setters (dung cho Edit car / Update status) ---
    void setName(const std::string& v) { name_ = v; }
    void setDescription(const std::string& v) { description_ = v; }
    void setSeats(int v) { seats_ = v; }
    void setTransmission(const std::string& v) { transmission_ = v; }
    void setFuel(const std::string& v) { fuel_ = v; }
    void setPricePerDay(long long v) { pricePerDay_ = v; }
    void setDiscountPercent(int v) { discountPercent_ = v; }
    void setStatus(CarStatus v) { status_ = v; }
    void setCategoryId(int v) { categoryId_ = v; }
    void setCounters(int rent, int view) { rentCount_ = rent; viewCount_ = view; }
    void increaseViewCount() { ++viewCount_; }
    void increaseRentCount() { ++rentCount_; }   // Nguoi 4 goi khi dat xe thanh cong

    // --- doc/ghi 1 dong file ---
    // id|name|description|seats|transmission|fuel|price|discount|status|rentCount|viewCount|categoryId
    std::string toLine() const;
    static bool fromLine(const std::string& line, Car& out);

    // Tra ve chuoi loi, rong neu hop le.
    std::string validate() const;

private:
    std::string id_, name_, description_, transmission_, fuel_;
    int seats_ = 0;
    long long pricePerDay_ = 0;
    int discountPercent_ = 0;
    CarStatus status_ = CarStatus::Ready;
    int rentCount_ = 0, viewCount_ = 0, categoryId_ = 0;
};

enum class SortKey { None, Price, Discount, RentCount };

class CarManager {
public:
    bool load(const std::string& path);
    bool save(const std::string& path) const;

    const std::vector<Car>& all() const { return cars_; }
    const Car* findById(const std::string& id) const;
    Car* findById(const std::string& id);

    // --- Use case ---
    std::vector<const Car*> byCategory(int categoryId) const;                 // Browse car
    std::vector<const Car*> search(const std::string& keyword) const;         // Search car
    std::vector<const Car*> listAll() const;                                  // View car list
    static void sortCars(std::vector<const Car*>& v, SortKey key, bool ascending);

    bool add(const Car& car, std::string& err);                               // Add car
    bool update(const Car& updated, std::string& err);                        // Edit car info (giu nguyen luot thue/xem)
    bool remove(const std::string& id);                                       // Delete car
    bool updateStatus(const std::string& id, CarStatus status, std::string& err); // Update car status

    bool hasCarsInCategory(int categoryId) const { return !byCategory(categoryId).empty(); }
    std::string generateId() const;                                           // C001, C002, ...
    void viewDetail(const std::string& id);                                   // tang luot xem

private:
    std::vector<Car> cars_;
};
