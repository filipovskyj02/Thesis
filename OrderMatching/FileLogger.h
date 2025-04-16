//
// Created by kuba on 4/6/25.
//

#ifndef FILELOGGER_H
#define FILELOGGER_H

#include <fstream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>
#include "Order.h"

class FileLogger {
public:
    void init(const std::string& filename);
    void close();
    void logOrder(const std::shared_ptr<Order>& order);
    void logCancel(OrderId canceledOrderId, long timestamp);

private:
    void loggingThreadFunc();

    std::ofstream outFile;
    std::thread loggingThread;
    std::mutex mutex;
    std::condition_variable cv;
    std::atomic<bool> running{true};

    std::queue<std::string> messageQueue;
};


#endif //FILELOGGER_H
