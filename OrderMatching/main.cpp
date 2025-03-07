#include <fstream>
#include <iostream>
#include <thread>
#include <sys/mman.h>
#include "Order.h"
#include "OrderBook.h"



int main() {
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
        perror("mlockall failed");
    }
    
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(2, &cpuset);
    if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0) {
        perror("pthread_setaffinity_np failed");
    }
    
    struct sched_param param;
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    auto orderBook = std::make_unique<OrderBook>();
    std::ifstream inputFile("../../data-gen/outputs/30000-500000-0.200000-0-02-03-2025 17-00-43.txt");

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
