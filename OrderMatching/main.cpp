#include <fstream>
#include <iostream>

#include "Order.h"
#include "OrderBook.h"



int main() {
    auto orderBook = std::make_unique<OrderBook>();
    std::ifstream inputFile("30000-1000000-23-24-15-02-2025.txt");
    if (!inputFile.is_open()) {
        throw std::runtime_error("Error opening file" );
    }
    int initialOrdersCount;
    int ordersCount;
    inputFile >> initialOrdersCount >> ordersCount;
    for (int i = 0; i < initialOrdersCount; i++) {
        int orderId;
        std::string side;
        std::string type;
        Price price;
        Volume volume;
        inputFile >> orderId >> side >> type >> price >> volume;
        auto order = std::make_shared<Order>(convertType(type), convertSide(side), price, volume);
        orderBook->placeOrder(order);
    }

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < ordersCount; i++) {
        OrderId orderId;
        std::string side = "";
        std::string type = "";
        Price price = 0;
        Volume volume = 0;
        inputFile >> orderId >> side;
        if (side == "CANCEL") {
            orderBook->cancelOrderLazy(orderBook->getOrder(orderId));
        }
        else {
            inputFile >> type;
            if (type == "MARKET") {
                inputFile >> volume;
                auto order = std::make_shared<Order>(convertType(type),convertSide(side), 100000.0, volume);
                orderBook->placeOrder(order);
            }
            else {
                inputFile >> price >> volume;
                auto order = std::make_shared<Order>(convertType(type),convertSide(side), price, volume);
                orderBook->placeOrder(order);
            }
        }
    }
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    std::cout << duration.count() << "ms" << std::endl;

    inputFile.close();

    return 0;
}
