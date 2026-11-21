PHAN NGUOI 2 - XE VA LOAI XE (C++17, console, luu file txt)

Bien dich va chay thu rieng:
  g++ -std=c++17 -o car_test test_car_main.cpp CarMenu.cpp Car.cpp Category.cpp
  ./car_test            (Windows: car_test.exe)

Chay tu thu muc co thu muc data/ (data/cars.txt, data/categories.txt).

Ghep vao main.cpp (Nguoi 1):
  #include "CarMenu.h"
  CarManager cars; CategoryManager cats;
  cats.load("data/categories.txt"); cars.load("data/cars.txt");
  guestCarMenu(cars, cats, "data/cars.txt");                           // Guest / Member
  adminCarMenu(cars, cats, "data/cars.txt", "data/categories.txt");    // Admin

Nguoi 4 can dung tu class Car:
  cars.findById(id), car->isAvailable(), car->finalPrice(), car->increaseRentCount()
  (sau khi doi trang thai/tang luot thue nho goi cars.save("data/cars.txt"))

Dinh dang cars.txt (phan cach bang '|'):
  id|ten|mo ta|so cho|hop so|nhien lieu|gia/ngay|giam gia %|Ready/Rented/Maintenance|luot thue|luot xem|ma loai
Dinh dang categories.txt:  id|ten loai
