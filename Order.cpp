#include "Order.h"
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

vector<Order> orders;   // danh sach don hang

// doc don hang tu file orders.txt
void loadOrders() {
    orders.clear();
    ifstream f("orders.txt");
    if (!f.is_open()) return;
    Order o;
    while (f >> o.id >> o.username >> o.carId >> o.total >> o.payment >> o.status) {
        orders.push_back(o);
    }
    f.close();
}

// ghi don hang ra file
void saveOrders() {
    ofstream f("orders.txt");
    for (int i = 0; i < orders.size(); i++) {
        f << orders[i].id << " " << orders[i].username << " " << orders[i].carId << " "
          << orders[i].total << " " << orders[i].payment << " " << orders[i].status << endl;
    }
    f.close();
}

// tim don theo id, khong co tra ve -1
int findOrder(int id) {
    for (int i = 0; i < orders.size(); i++) {
        if (orders[i].id == id) return i;
    }
    return -1;
}

void showOrder(Order o) {
    cout << o.id << " | " << o.username << " | " << o.carId << " | "
         << o.total << " | " << o.payment << " | " << o.status << endl;
}

int addOrder(string username, string carId, double total, string payment) {
    Order o;
    o.id = 1;
    for (int i = 0; i < orders.size(); i++) {
        if (orders[i].id >= o.id) o.id = orders[i].id + 1;
    }
    o.username = username;
    o.carId = carId;
    o.total = total;
    o.payment = payment;
    o.status = "Pending";
    orders.push_back(o);
    saveOrders();
    return o.id;
}

// xem tat ca don (Admin)
void viewOrderList() {
    if (orders.size() == 0) {
        cout << "Chua co don hang!" << endl;
        return;
    }
    cout << "ID | User | Xe | Tong tien | Thanh toan | Trang thai" << endl;
    for (int i = 0; i < orders.size(); i++) showOrder(orders[i]);
}

// xem don cua 1 user (Member)
void viewMyOrders(string username) {
    bool found = false;
    for (int i = 0; i < orders.size(); i++) {
        if (orders[i].username == username) {
            showOrder(orders[i]);
            found = true;
        }
    }
    if (!found) cout << "Ban chua co don hang!" << endl;
}

void viewOrder(int id) {
    int pos = findOrder(id);
    if (pos == -1) {
        cout << "Khong tim thay don!" << endl;
        return;
    }
    showOrder(orders[pos]);
}

// Member huy don cua minh (chi khi con Pending)
void cancelOrder(int id, string username) {
    int pos = findOrder(id);
    if (pos == -1) {
        cout << "Khong tim thay don!" << endl;
    } else if (orders[pos].username != username) {
        cout << "Day khong phai don cua ban!" << endl;
    } else if (orders[pos].status != "Pending") {
        cout << "Chi huy duoc don Pending!" << endl;
    } else {
        orders[pos].status = "Cancelled";
        saveOrders();
        cout << "Da huy don." << endl;
    }
}

void confirmOrder(int id) {
    int pos = findOrder(id);
    if (pos == -1) {
        cout << "Khong tim thay don!" << endl;
    } else if (orders[pos].status != "Pending") {
        cout << "Chi xac nhan duoc don Pending!" << endl;
    } else {
        orders[pos].status = "Confirmed";
        saveOrders();
        cout << "Da xac nhan don." << endl;
    }
}

void updateStatus(int id, string newStatus) {
    int pos = findOrder(id);
    if (pos == -1) {
        cout << "Khong tim thay don!" << endl;
    } else if (newStatus != "Pending" && newStatus != "Confirmed" && newStatus != "Delivering"
               && newStatus != "Completed" && newStatus != "Cancelled") {
        cout << "Trang thai khong hop le!" << endl;
    } else if (orders[pos].status == "Completed" || orders[pos].status == "Cancelled") {
        cout << "Don da ket thuc, khong doi duoc!" << endl;
    } else {
        orders[pos].status = newStatus;
        saveOrders();
        cout << "Da cap nhat trang thai." << endl;
    }
}

// Admin huy don bat ky
void adminCancelOrder(int id) {
    int pos = findOrder(id);
    if (pos == -1) {
        cout << "Khong tim thay don!" << endl;
    } else if (orders[pos].status == "Completed" || orders[pos].status == "Cancelled") {
        cout << "Don da ket thuc, khong huy duoc!" << endl;
    } else {
        orders[pos].status = "Cancelled";
        saveOrders();
        cout << "Admin da huy don." << endl;
    }
}

void deleteOrder(int id) {
    int pos = findOrder(id);
    if (pos == -1) {
        cout << "Khong tim thay don!" << endl;
    } else {
        orders.erase(orders.begin() + pos);
        saveOrders();
        cout << "Da xoa don." << endl;
    }
}

void memberOrderMenu(string username) {
    int choice, id;
    do {
        cout << "\n--- DON HANG (MEMBER) ---" << endl;
        cout << "1. Xem don cua toi" << endl;
        cout << "2. Xem chi tiet don" << endl;
        cout << "3. Huy don" << endl;
        cout << "0. Quay lai" << endl;
        cout << "Chon: ";
        cin >> choice;
        if (choice == 1) {
            viewMyOrders(username);
        } else if (choice == 2) {
            cout << "Nhap ID: ";
            cin >> id;
            viewOrder(id);
        } else if (choice == 3) {
            cout << "Nhap ID don can huy: ";
            cin >> id;
            cancelOrder(id, username);
        }
    } while (choice != 0);
}

void adminOrderMenu() {
    int choice, id;
    string status;
    do {
        cout << "\n--- DON HANG (ADMIN) ---" << endl;
        cout << "1. Xem tat ca don" << endl;
        cout << "2. Xem chi tiet don" << endl;
        cout << "3. Xac nhan don" << endl;
        cout << "4. Cap nhat trang thai" << endl;
        cout << "5. Huy don" << endl;
        cout << "6. Xoa don" << endl;
        cout << "0. Quay lai" << endl;
        cout << "Chon: ";
        cin >> choice;
        if (choice == 1) {
            viewOrderList();
        } else if (choice >= 2 && choice <= 6) {
            cout << "Nhap ID don: ";
            cin >> id;
            if (choice == 2) viewOrder(id);
            else if (choice == 3) confirmOrder(id);
            else if (choice == 4) {
                cout << "Trang thai moi (Pending/Confirmed/Delivering/Completed/Cancelled): ";
                cin >> status;
                updateStatus(id, status);
            }
            else if (choice == 5) adminCancelOrder(id);
            else if (choice == 6) deleteOrder(id);
        }
    } while (choice != 0);
}