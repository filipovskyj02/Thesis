#include "KDBLogger.h"

#include <iostream>
#include <sstream>

constexpr size_t BATCH_SIZE = 50000;
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
            return orderQueue.size() >= BATCH_SIZE ||
                   cancelQueue.size() >= BATCH_SIZE ||
                   (!running && (!orderQueue.empty() || !cancelQueue.empty()));
        });

        if (!cancelQueue.empty() && (cancelQueue.size() >= BATCH_SIZE || !running)) {
            size_t batchSize = std::min(BATCH_SIZE, cancelQueue.size());

            std::ostringstream timestamps, orderIds;
            timestamps << "(";
            orderIds << "(";

            for (size_t i = 0; i < batchSize; ++i) {
                auto [orderId, timestamp] = cancelQueue.front();
                cancelQueue.pop();

                timestamps << timestamp;
                orderIds << orderId;

                if (i < batchSize - 1) {
                    timestamps << " ";
                    orderIds << " ";
                }
            }

            timestamps << ")";
            orderIds << ")";

            std::ostringstream query;
            query << "`cancels insert (" << timestamps.str() << "; " << orderIds.str() << ")";

            lock.unlock();
            k(-conn, (S)query.str().c_str(), (K)0);
            lock.lock();
        }

        if (!orderQueue.empty() && (orderQueue.size() >= BATCH_SIZE || !running)) {
            size_t batchSize = std::min(BATCH_SIZE, orderQueue.size());

            std::ostringstream ids, timestamps, sides, orderTypes, prices, volumes;
            ids << "(";
            timestamps << "(";
            sides << "(";
            orderTypes << "(";
            prices << "(";
            volumes << "(";

            for (size_t i = 0; i < batchSize; ++i) {
                auto order = orderQueue.front();
                orderQueue.pop();

                ids << order->getId();
                timestamps << order->getTimestamp();
                sides << "`" << (order->getSide() == Side::BUY ? "BUY" : "SELL");
                orderTypes << "`" << (order->getOrderType() == OrderType::LIMIT ? "LIMIT" : "MARKET");
                prices << order->getPrice() << "e";
                volumes << order->getOriginalVolume();

                if (i < batchSize - 1) {
                    ids << " ";
                    timestamps << " ";
                    sides << " ";
                    orderTypes << " ";
                    prices << " ";
                    volumes << " ";
                }
            }

            ids << ")";
            timestamps << ")";
            sides << ")";
            orderTypes << ")";
            prices << ")";
            volumes << ")";

            std::ostringstream query;
            query << "`orders insert (" << ids.str() << "; "
                  << timestamps.str() << "; "
                  << sides.str() << "; "
                  << orderTypes.str() << "; "
                  << prices.str() << "; "
                  << volumes.str() << ")";

            lock.unlock();
            k(-conn, (S)query.str().c_str(), (K)0);
            lock.lock();
        }
    }
}
