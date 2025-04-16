//
// Created by kuba on 4/6/25.
//

#include "FileLogger.h"

#include <iostream>
#include <sstream>
#include <chrono>

void FileLogger::init(const std::string& filename) {
    outFile.open(filename, std::ios::out | std::ios::trunc);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
        return;
    }
    loggingThread = std::thread(&FileLogger::loggingThreadFunc, this);
}

void FileLogger::close() {
    running = false;
    cv.notify_all();
    if (loggingThread.joinable()) loggingThread.join();
    if (outFile.is_open()) outFile.close();
}

void FileLogger::logOrder(const std::shared_ptr<Order>& order) {
    std::ostringstream ss;
    ss << "ORDER "
       << order->getId() << " "
       << order->getTimestamp() << " "
       << (order->getSide() == Side::BUY ? "BUY" : "SELL") << " "
       << (order->getOrderType() == OrderType::LIMIT ? "LIMIT" : "MARKET") << " "
       << order->getPrice() << " "
       << order->getOriginalVolume();

    {
        std::lock_guard<std::mutex> lock(mutex);
        messageQueue.push(ss.str());
    }
    cv.notify_one();
}

void FileLogger::logCancel(OrderId canceledOrderId, long timestamp) {
    std::ostringstream ss;
    ss << "CANCEL "
       << timestamp << " "
       << canceledOrderId;

    {
        std::lock_guard<std::mutex> lock(mutex);
        messageQueue.push(ss.str());
    }
    cv.notify_one();
}

void FileLogger::loggingThreadFunc() {
    while (running || !messageQueue.empty()) {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this] {
            return !messageQueue.empty() || !running;
        });

        while (!messageQueue.empty()) {
            const std::string& msg = messageQueue.front();
            outFile << msg << "\n";
            outFile.flush();
            messageQueue.pop();
        }
    }
}