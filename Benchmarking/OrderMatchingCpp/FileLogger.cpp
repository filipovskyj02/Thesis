#include "FileLogger.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>


void FileLogger::init(const std::string& filename) {

    loggingThread = std::thread(&FileLogger::loggingThreadFunc, this, filename);
}

void FileLogger::close() {
    running = false;
    cv.notify_all();
    if (loggingThread.joinable()) loggingThread.join();
    if (outFile.is_open()) outFile.close();
}

void FileLogger::logOrder(const std::shared_ptr<Order>& order) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        orderQueue.push(order);
    }
    cv.notify_one();
}

void FileLogger::logCancel(OrderId canceledOrderId, long timestamp) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        cancelQueue.emplace(canceledOrderId, timestamp);
    }
    cv.notify_one();
}

void FileLogger::loggingThreadFunc(const std::string& filename) {
    outFile.open(filename, std::ios::out | std::ios::trunc);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
        return;
    }
    std::vector<std::shared_ptr<Order>> orderBatch;
    std::vector<std::pair<OrderId, long>> cancelBatch;
    const auto flushInterval = std::chrono::milliseconds(1);
    const size_t maxBatchSize = 1000;

    while (running || !orderQueue.empty() || !cancelQueue.empty()) {
        {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait_for(lock, flushInterval, [this] {
                return !orderQueue.empty() || !cancelQueue.empty() || !running;
            });

            while (!orderQueue.empty() && orderBatch.size() < maxBatchSize) {
                orderBatch.push_back(orderQueue.front());
                orderQueue.pop();
            }

            while (!cancelQueue.empty() && cancelBatch.size() < maxBatchSize) {
                cancelBatch.push_back(cancelQueue.front());
                cancelQueue.pop();
            }
        }

        if (!orderBatch.empty()) {
            flushOrderBatch(orderBatch);
            orderBatch.clear();
        }

        if (!cancelBatch.empty()) {
            flushCancelBatch(cancelBatch);
            cancelBatch.clear();
        }
    }
}

void FileLogger::flushOrderBatch(std::vector<std::shared_ptr<Order>>& batch) {
    if (batch.empty()) return;

    for (const auto& o : batch) {
        outFile << "ORDER "
                << o->getId() << " "
                << o->getTimestamp() << " "
                << (o->getSide() == Side::BUY ? "BUY" : "SELL") << " "
                << (o->getOrderType() == OrderType::LIMIT ? "LIMIT" : "MARKET") << " "
                << o->getPrice() << " "
                << o->getOriginalVolume() << "\n";
    }
    outFile.flush();
}

void FileLogger::flushCancelBatch(std::vector<std::pair<OrderId, long>>& batch) {
    if (batch.empty()) return;

    for (const auto& cancel : batch) {
        outFile << "CANCEL " << cancel.second << " " << cancel.first << "\n";
    }
    outFile.flush();
}
