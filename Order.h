#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <vector>
using namespace std;

struct Order {
    int id;
    string username;
    string carId;
    double total;
    string payment;   // Cash hoac Online
    string status;    // Pending, Confirmed, Delivering, Completed, Cancelled
};

void loadOrders();
const vector<Order>& getAllOrders();                 // doc file khi mo chuong trinh
int addOrder(string username, string carId, double total, string payment); // Nguoi 4 goi
void memberOrderMenu(string username);  // Nguoi 1 goi cho Member
void adminOrderMenu();                  // Nguoi 1 goi cho Admin

#endif
