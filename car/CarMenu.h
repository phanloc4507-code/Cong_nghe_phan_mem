#pragma once
#include "Car.h"
#include "Category.h"

// Cac menu cua Nguoi 2. Nguoi 1 chi can goi trong main.cpp:
//   guestCarMenu(...)  -> Guest / Member: Browse car, Search car (luu luot xem vao carPath)
//   adminCarMenu(...)  -> Admin: View list, Add, Edit, Delete, Update status, Quan ly loai xe
// Moi thay doi deu duoc luu ngay xuong file (carPath, categoryPath).
void guestCarMenu(CarManager& cars, CategoryManager& cats, const std::string& carPath);
void adminCarMenu(CarManager& cars, CategoryManager& cats,
                  const std::string& carPath, const std::string& categoryPath);
