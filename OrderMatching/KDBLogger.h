//
// Created by kuba on 4/6/25.
//

#ifndef KDBLOGGER_H
#define KDBLOGGER_H
#include <memory>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "Order.h"
#include "k.h"

class KDBLogger {
public:
    void init();
    void close();
    void logOrder(const std::shared_ptr<Order>& order);
    void logCancel(OrderId canceledOrderId, long timestamp);

private:
    void loggingThreadFunc();

    std::queue<std::shared_ptr<Order>> orderQueue;
    std::queue<std::pair<OrderId, long>> cancelQueue;
    std::mutex mutex;
    std::condition_variable cv;
    std::atomic<bool> running{true};
    std::thread loggingThread;
    I conn;;

    void initConnection();
};

#endif //KDBLOGGER_H
