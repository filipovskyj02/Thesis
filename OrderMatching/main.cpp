#include <fstream>
#include <iostream>
#include <thread>

#include "Order.h"
#include "OrderBook.h"



int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    auto orderBook = std::make_unique<OrderBook>();
    std::ifstream inputFile("../../data-gen/outputs/30000-2500000-08-05-02-03-2025.txt");
    if (!inputFile.is_open()) {
        throw std::runtime_error("Error opening file" );
    }
    int initialOrdersCount;
    int ordersCount;
    inputFile >> initialOrdersCount >> ordersCount;
    OrderId orderId;
    std::string side;
    std::string type;
    Price price;
    Volume volume = 0;
    for (int i = 0; i < initialOrdersCount; i++) {
        inputFile >> orderId >> side >> type >> price >> volume;
        auto order = std::make_shared<Order>(convertSide(side), convertType(type), price, volume);
        orderBook->placeOrder(order);
    }

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < ordersCount; i++) {
        inputFile >> orderId >> side >> type >> price >> volume;
        if (type == "CANCEL") {
            orderBook->cancelOrderLazy(orderBook->getOrder(orderId));
        }
        else {
            if (type == "MARKET") {
                auto order = std::make_shared<Order>(convertSide(side), convertType(type), volume);
                orderBook->placeOrder(order);
            }
            else {
                auto order = std::make_shared<Order>(convertSide(side), convertType(type), price, volume);
                orderBook->placeOrder(order);
            }

        }
       // std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    std::cout << duration.count() << "ms" << std::endl;

    inputFile.close();
    orderBook->countFilledCount();
    return 0;
}
