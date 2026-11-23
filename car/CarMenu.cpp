#include "CarMenu.h"
#include "TextUtil.h"
#include <iomanip>
#include <iostream>
#include <limits>

namespace {

const size_t PAGE_SIZE = 10;

// ---------- nhap lieu ----------
std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return trim(s);
}

// Doc so nguyen trong [lo, hi]; nhap sai thi hoi lai. Tra ve false neu het du lieu (EOF).
bool readInt(const std::string& prompt, long long lo, long long hi, long long& out,
             const long long* keepIfEmpty = nullptr) {
    while (true) {
        std::cout << prompt;
        std::string s;
        if (!std::getline(std::cin, s)) return false;
        s = trim(s);
        if (s.empty() && keepIfEmpty) { out = *keepIfEmpty; return true; }   // Enter = giu gia tri cu
        try {
            size_t pos = 0;
            long long v = std::stoll(s, &pos);
            if (pos == s.size() && v >= lo && v <= hi) { out = v; return true; }
        } catch (...) {}
        std::cout << "  Vui long nhap so nguyen tu " << lo << " den " << hi << ".\n";
    }
}

bool confirm(const std::string& question) {
    std::string a = toLower(readLine(question + " (y/n): "));
    return a == "y" || a == "yes";
}

std::string money(long long v) {
    std::string s = std::to_string(v), out;
    int cnt = 0;
    for (int i = static_cast<int>(s.size()) - 1; i >= 0; --i) {
        out.insert(out.begin(), s[i]);
        if (++cnt % 3 == 0 && i > 0) out.insert(out.begin(), '.');
    }
    return out;
}

// ---------- hien thi ----------
void printCarRow(const Car& c, const CategoryManager& cats) {
    std::cout << std::left << std::setw(6) << c.id() << std::setw(26) << c.name().substr(0, 25)
              << std::setw(14) << cats.nameOf(c.categoryId()).substr(0, 13)
              << std::right << std::setw(12) << money(c.pricePerDay())
              << std::setw(6) << (std::to_string(c.discountPercent()) + "%")
              << std::setw(7) << c.rentCount() << "  "
              << std::left << std::setw(12) << statusToString(c.status()) << "\n";
}

void printHeader() {
    std::cout << std::left << std::setw(6) << "Ma" << std::setw(26) << "Ten xe"
              << std::setw(14) << "Loai" << std::right << std::setw(12) << "Gia/ngay"
              << std::setw(6) << "Giam" << std::setw(7) << "Luot" << "  "
              << std::left << std::setw(12) << "Trang thai" << "\n"
              << std::string(83, '-') << "\n";
}

void printCarDetail(const Car& c, const CategoryManager& cats) {
    std::cout << "\n===== CHI TIET XE =====\n"
              << "Ma xe       : " << c.id() << "\n"
              << "Ten xe      : " << c.name() << "\n"
              << "Loai xe     : " << cats.nameOf(c.categoryId()) << "\n"
              << "Mo ta       : " << c.description() << "\n"
              << "So cho      : " << c.seats() << "\n"
              << "Hop so      : " << c.transmission() << "\n"
              << "Nhien lieu  : " << c.fuel() << "\n"
              << "Gia/ngay    : " << money(c.pricePerDay()) << " VND\n"
              << "Giam gia    : " << c.discountPercent() << "%  ->  gia sau giam "
              << money(c.finalPrice()) << " VND\n"
              << "Trang thai  : " << statusToString(c.status()) << "\n"
              << "Luot thue   : " << c.rentCount() << "\n"
              << "Luot xem    : " << c.viewCount() << "\n";
}

void listCategories(const CategoryManager& cats) {
    std::cout << "\nDanh sach loai xe:\n";
    if (cats.all().empty()) std::cout << "  (chua co loai xe nao)\n";
    for (const auto& c : cats.all()) std::cout << "  " << c.id << ". " << c.name << "\n";
}

// Chon loai xe, tra ve id (0 = huy)
int chooseCategory(const CategoryManager& cats) {
    listCategories(cats);
    if (cats.all().empty()) return 0;
    while (true) {
        long long id;
        if (!readInt("Chon ma loai xe (0 = huy): ", 0, 1000000, id)) return 0;
        if (id == 0 || cats.findById(static_cast<int>(id))) return static_cast<int>(id);
        std::cout << "  Khong co loai xe nay.\n";
    }
}

bool chooseSort(SortKey& key, bool& asc) {
    std::cout << "\nSap xep theo: 1) Gia/ngay  2) % giam gia  3) So luot thue  0) Khong sap xep\n";
    long long k;
    if (!readInt("Chon: ", 0, 3, k)) return false;
    key = static_cast<SortKey>(k);                    // 0..3 trung voi thu tu enum
    if (key != SortKey::None) {
        long long o;
        if (!readInt("1) Tang dan  2) Giam dan: ", 1, 2, o)) return false;
        asc = (o == 1);
    }
    return true;
}

// Hien thi danh sach co phan trang; cho phep xem chi tiet (tang luot xem).
void showCarList(CarManager& cars, const CategoryManager& cats, std::vector<const Car*> list,
                 SortKey key, bool asc) {
    if (list.empty()) { std::cout << "\nKhong tim thay xe nao.\n"; return; }
    CarManager::sortCars(list, key, asc);
    size_t page = 0, pages = (list.size() + PAGE_SIZE - 1) / PAGE_SIZE;
    while (true) {
        std::cout << "\n--- Trang " << page + 1 << "/" << pages << " (" << list.size() << " xe) ---\n";
        printHeader();
        for (size_t i = page * PAGE_SIZE; i < std::min(list.size(), (page + 1) * PAGE_SIZE); ++i)
            printCarRow(*list[i], cats);
        std::string cmd = toLower(readLine("\n[n] trang sau  [p] trang truoc  [ma xe] xem chi tiet  [q] quay lai: "));
        if (cmd == "q" || cmd.empty()) return;
        if (cmd == "n") { if (page + 1 < pages) ++page; else std::cout << "  Da o trang cuoi.\n"; }
        else if (cmd == "p") { if (page > 0) --page; else std::cout << "  Dang o trang dau.\n"; }
        else if (const Car* c = cars.findById(cmd)) {
            cars.viewDetail(c->id());
            printCarDetail(*c, cats);
        } else std::cout << "  Lenh hoac ma xe khong hop le.\n";
    }
}

// ---------- nhap thong tin xe ----------
bool inputCarFields(Car& c, const CategoryManager& cats, bool editing) {
    auto keepOrNew = [&](const std::string& label, const std::string& oldVal) {
        std::string s = readLine(label + (editing ? " [" + oldVal + "]: " : ": "));
        return (editing && s.empty()) ? oldVal : sanitize(s);
    };
    c.setName(keepOrNew("Ten xe", c.name()));
    c.setDescription(keepOrNew("Mo ta", c.description()));
    c.setTransmission(keepOrNew("Hop so (So san/So tu dong)", c.transmission()));
    c.setFuel(keepOrNew("Nhien lieu", c.fuel()));

    long long v, oldSeats = c.seats(), oldPrice = c.pricePerDay(), oldDisc = c.discountPercent();
    if (!readInt("So cho ngoi" + std::string(editing ? " [" + std::to_string(oldSeats) + "]" : "") + ": ", 1, 100, v, editing ? &oldSeats : nullptr)) return false;
    c.setSeats(static_cast<int>(v));
    if (!readInt("Gia thue/ngay (VND)" + std::string(editing ? " [" + std::to_string(oldPrice) + "]" : "") + ": ", 1, 1000000000LL, v, editing ? &oldPrice : nullptr)) return false;
    c.setPricePerDay(v);
    if (!readInt("Giam gia (%)" + std::string(editing ? " [" + std::to_string(oldDisc) + "]" : "") + ": ", 0, 100, v, editing ? &oldDisc : nullptr)) return false;
    c.setDiscountPercent(static_cast<int>(v));

    std::cout << (editing ? "Chon loai xe moi (0 = giu nguyen loai cu):" : "Chon loai xe:");
    int cat = chooseCategory(cats);
    if (cat == 0) {
        if (editing) return true;            // giu nguyen loai cu
        std::cout << "  Phai chon loai xe. Huy thao tac.\n";
        return false;
    }
    c.setCategoryId(cat);
    return true;
}

// ---------- chuc nang Admin ----------
void adminAddCar(CarManager& cars, const CategoryManager& cats, const std::string& path) {
    std::cout << "\n== THEM XE MOI ==\n";
    Car c;
    std::string id = readLine("Ma xe (Enter = tu dong " + cars.generateId() + "): ");
    id = id.empty() ? cars.generateId() : sanitize(id);
    c = Car(id, "", "", 0, "", "", 0, 0, CarStatus::Ready, 0);
    if (!inputCarFields(c, cats, false)) return;
    std::string err;
    if (!cars.add(c, err)) { std::cout << "Loi: " << err << " Thao tac bi huy.\n"; return; }
    cars.save(path);
    std::cout << "Da them xe " << c.id() << " thanh cong.\n";
}

void adminEditCar(CarManager& cars, const CategoryManager& cats, const std::string& path) {
    std::cout << "\n== SUA THONG TIN XE ==\n";
    const Car* cur = cars.findById(readLine("Nhap ma xe can sua: "));
    if (!cur) { std::cout << "Khong tim thay xe.\n"; return; }
    printCarDetail(*cur, cats);
    std::cout << "\n(Enter de giu nguyen gia tri trong [ ])\n";
    Car edited = *cur;
    if (!inputCarFields(edited, cats, true)) return;
    std::string err;
    if (!cars.update(edited, err)) { std::cout << "Loi: " << err << " Du lieu khong thay doi.\n"; return; }
    cars.save(path);
    std::cout << "Da cap nhat xe " << edited.id() << ".\n";
}

void adminDeleteCar(CarManager& cars, const CategoryManager& cats, const std::string& path) {
    std::cout << "\n== XOA XE ==\n";
    const Car* cur = cars.findById(readLine("Nhap ma xe can xoa: "));
    if (!cur) { std::cout << "Khong tim thay xe.\n"; return; }
    std::string id = cur->id();
    printCarDetail(*cur, cats);
    if (!confirm("Ban chac chan muon xoa xe nay?")) { std::cout << "Da huy, xe khong bi xoa.\n"; return; }
    cars.remove(id);
    cars.save(path);
    std::cout << "Da xoa xe " << id << ".\n";
}

void adminUpdateStatus(CarManager& cars, const std::string& path) {
    std::cout << "\n== CAP NHAT TRANG THAI XE ==\n";
    const Car* cur = cars.findById(readLine("Nhap ma xe: "));
    if (!cur) { std::cout << "Khong tim thay xe.\n"; return; }
    std::string id = cur->id();
    std::cout << "Xe " << id << " - " << cur->name() << ", trang thai hien tai: "
              << statusToString(cur->status()) << "\n"
              << "Trang thai moi: 1) Ready (san sang)  2) Rented (dang thue)  3) Maintenance (bao tri)  0) Huy\n";
    long long s;
    if (!readInt("Chon: ", 0, 3, s) || s == 0) { std::cout << "Da huy, trang thai khong doi.\n"; return; }
    std::string err;
    cars.updateStatus(id, static_cast<CarStatus>(s - 1), err);
    cars.save(path);
    std::cout << "Da cap nhat trang thai xe " << id << ".\n";
}

void adminViewList(CarManager& cars, const CategoryManager& cats, const std::string& path) {
    std::cout << "\n== DANH SACH XE ==\n1) Tat ca xe  2) Theo loai xe\n";
    long long m;
    if (!readInt("Chon: ", 1, 2, m)) return;
    std::vector<const Car*> list;
    if (m == 1) list = cars.listAll();
    else {
        int cat = chooseCategory(cats);
        if (cat == 0) return;
        list = cars.byCategory(cat);
    }
    SortKey key = SortKey::None; bool asc = true;
    if (!chooseSort(key, asc)) return;
    showCarList(cars, cats, list, key, asc);
    cars.save(path);                          // luu luot xem
}

void adminCategoryMenu(CarManager& cars, CategoryManager& cats, const std::string& path) {
    while (true) {
        std::cout << "\n== QUAN LY LOAI XE ==\n1) Xem danh sach  2) Them  3) Doi ten  4) Xoa  0) Quay lai\n";
        long long ch;
        if (!readInt("Chon: ", 0, 4, ch) || ch == 0) return;
        std::string err;
        if (ch == 1) listCategories(cats);
        else if (ch == 2) {
            if (cats.add(readLine("Ten loai xe moi: "), err)) { cats.save(path); std::cout << "Da them loai xe.\n"; }
            else std::cout << "Loi: " << err << "\n";
        } else if (ch == 3) {
            int id = chooseCategory(cats);
            if (id == 0) continue;
            if (cats.rename(id, readLine("Ten moi: "), err)) { cats.save(path); std::cout << "Da doi ten.\n"; }
            else std::cout << "Loi: " << err << "\n";
        } else if (ch == 4) {
            int id = chooseCategory(cats);
            if (id == 0) continue;
            if (cars.hasCarsInCategory(id)) { std::cout << "Khong the xoa: van con xe thuoc loai nay.\n"; continue; }
            if (confirm("Xoa loai xe nay?")) { cats.remove(id); cats.save(path); std::cout << "Da xoa loai xe.\n"; }
        }
    }
}

} // namespace

// ---------- ham cong khai ----------
void guestCarMenu(CarManager& cars, CategoryManager& cats, const std::string& carPath) {
    while (true) {
        std::cout << "\n===== XEM / TIM XE =====\n1) Xem xe theo loai (Browse car)\n2) Tim xe theo tu khoa (Search car)\n0) Quay lai\n";
        long long ch;
        if (!readInt("Chon: ", 0, 2, ch) || ch == 0) return;
        std::vector<const Car*> list;
        if (ch == 1) {
            int cat = chooseCategory(cats);
            if (cat == 0) continue;
            list = cars.byCategory(cat);
        } else {
            std::string kw = readLine("Nhap tu khoa (ten xe, hang, mau): ");
            if (kw.empty()) { std::cout << "Tu khoa khong duoc de trong.\n"; continue; }
            list = cars.search(kw);
        }
        SortKey key = SortKey::None; bool asc = true;
        if (!list.empty() && !chooseSort(key, asc)) return;
        showCarList(cars, cats, list, key, asc);
        cars.save(carPath);                       // luu luot xem
    }
}

void adminCarMenu(CarManager& cars, CategoryManager& cats,
                  const std::string& carPath, const std::string& categoryPath) {
    while (true) {
        std::cout << "\n===== ADMIN - QUAN LY XE =====\n"
                     "1) Xem danh sach xe\n2) Them xe\n3) Sua thong tin xe\n4) Xoa xe\n"
                     "5) Cap nhat trang thai xe\n6) Quan ly loai xe\n0) Quay lai\n";
        long long ch;
        if (!readInt("Chon: ", 0, 6, ch) || ch == 0) return;
        switch (ch) {
            case 1: adminViewList(cars, cats, carPath); break;
            case 2: adminAddCar(cars, cats, carPath); break;
            case 3: adminEditCar(cars, cats, carPath); break;
            case 4: adminDeleteCar(cars, cats, carPath); break;
            case 5: adminUpdateStatus(cars, carPath); break;
            case 6: adminCategoryMenu(cars, cats, categoryPath); break;
        }
    }
}
