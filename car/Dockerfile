FROM gcc:13
WORKDIR /app
COPY . .
RUN g++ -std=c++17 -o car_test test_car_main.cpp Car.cpp Category.cpp CarMenu.cpp
CMD ["./car_test"]
