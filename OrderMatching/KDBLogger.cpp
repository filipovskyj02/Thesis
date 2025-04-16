//
// Created by kuba on 4/6/25.
//

#include "KDBLogger.h"

#include <iostream>
#include <sstream>

void KDBLogger::initConnection() {
    conn = khpu((S)"localhost", 5000, (S)"");
    if (conn < 0) {
        std::cerr << "Failed to connect to kdb+" << std::endl;
    }
}

void KDBLogger::init() {
    initConnection();
    loggingThread = std::thread(&KDBLogger::loggingThreadFunc, this);
}

void KDBLogger::close() {
    running = false;
    cv.notify_all();
    if (loggingThread.joinable()) loggingThread.join();
    if (conn) {
        kclose(conn);
    }
}

void KDBLogger::logOrder(const std::shared_ptr<Order>& order) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        orderQueue.push(order);
    }
    cv.notify_one();
}

void KDBLogger::logCancel(OrderId canceledOrderId, long timestamp) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        cancelQueue.emplace(canceledOrderId, timestamp);
    }
    cv.notify_one();
}

void KDBLogger::loggingThreadFunc() {
    while (running || !orderQueue.empty() || !cancelQueue.empty()) {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this] {
            return !orderQueue.empty() || !cancelQueue.empty() || !running;
        });

        while (!orderQueue.empty()) {
            auto order = orderQueue.front();
            orderQueue.pop();
            lock.unlock();

            std::ostringstream query;
            query << "`orders insert ("
                  << order->getId() << "; "
                  << order->getTimestamp() << "; "
                  << "`" << (order->getSide() == Side::BUY ? "BUY" : "SELL") << "; "
                  << "`" << (order->getOrderType() == OrderType::LIMIT ? "LIMIT" : "MARKET") << "; "
                  << order->getPrice() << "e; "
                  << order->getOriginalVolume() << ")";

            K result = k(conn, (S)query.str().c_str(), (K)0);
            if (!result || result->t == -128)
                std::cerr << "Order insert failed: " << (result ? result->s : "NULL") << "\n";
            r0(result);

            lock.lock();
        }

        while (!cancelQueue.empty()) {
            auto [orderId, timestamp] = cancelQueue.front();
            cancelQueue.pop();
            lock.unlock();

            std::ostringstream query;
            query << "`cancels insert ("
                  << timestamp << "; "
                  << orderId << ")";

            K result = k(conn, (S)query.str().c_str(), (K)0);
            if (!result || result->t == -128)
                std::cerr << "Cancel insert failed: " << (result ? result->s : "NULL") << "\n";
            r0(result);

            lock.lock();
        }
    }
}
