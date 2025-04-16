#include <fstream>
#include <iostream>
#include <thread>
#include <sys/mman.h>
#include "Order.h"
#include "OrderBook.h"
#include "PostgresLogger.h"
#include "PostgresThreadedLogger.h"
#include "FileLogger.h"


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

    std::ifstream inputFile("../../data-gen/outputs/1000-10000-0.670000-0.950000-05-04-2025 18-22-30.txt");

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

    //KDBLogger logger;
    //PostgresLogger logger;
    //PostgresThreadedLogger logger;
    FileLogger logger;
    logger.init("log");

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < ordersCount; i++) {
        inputFile >> orderId >> side >> type >> price >> volume;
        if (type == "CANCEL") {
            logger.logCancel(orderId,std::chrono::duration_cast<std::chrono::microseconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count());
            orderBook->cancelOrderLazy(orderBook->getOrder(orderId));
        }
        else {
            if (type == "MARKET") {
                auto order = std::make_shared<Order>(convertSide(side), convertType(type), volume);
                logger.logOrder(order);
                orderBook->placeOrder(order);
            }
            else {
                auto order = std::make_shared<Order>(convertSide(side), convertType(type), price, volume);
                logger.logOrder(order);
                orderBook->placeOrder(order);
            }

        }
    }
    inputFile.close();
    orderBook->countFilledCount();

    auto matchingDone = std::chrono::steady_clock::now();

    logger.close();
    auto totalDone = std::chrono::steady_clock::now();

    auto matchingDuration = matchingDone - start;
    auto matchingSec = std::chrono::duration_cast<std::chrono::seconds>(matchingDuration);
    auto matchingMs = std::chrono::duration_cast<std::chrono::milliseconds>(matchingDuration - matchingSec);
    auto matchingUs = std::chrono::duration_cast<std::chrono::microseconds>(matchingDuration - matchingSec - matchingMs);

    std::cout << "Matching duration: "
              << matchingSec.count() << "s "
              << matchingMs.count() << "ms "
              << matchingUs.count() << "us\n";

    auto totalDuration = totalDone - start;
    auto totalSec = std::chrono::duration_cast<std::chrono::seconds>(totalDuration);
    auto totalMs = std::chrono::duration_cast<std::chrono::milliseconds>(totalDuration - totalSec);
    auto totalUs = std::chrono::duration_cast<std::chrono::microseconds>(totalDuration - totalSec - totalMs);

    std::cout << "Total duration: "
              << totalSec.count() << "s "
              << totalMs.count() << "ms "
              << totalUs.count() << "us\n";

    auto diff = totalDuration - matchingDuration;
    auto diffSec = std::chrono::duration_cast<std::chrono::seconds>(diff);
    auto diffMs = std::chrono::duration_cast<std::chrono::milliseconds>(diff - diffSec);
    auto diffUs = std::chrono::duration_cast<std::chrono::microseconds>(diff - diffSec - diffMs);

    std::cout << "Logging thread after matching: "
              << diffSec.count() << "s "
              << diffMs.count() << "ms "
              << diffUs.count() << "us\n";


    return 0;
}
