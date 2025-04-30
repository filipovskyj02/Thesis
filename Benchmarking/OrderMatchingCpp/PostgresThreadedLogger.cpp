//
// Created by kuba on 4/5/25.
//

#include "PostgresThreadedLogger.h"

#include <sstream>
#include <iostream>
#include <chrono>

void PostgresThreadedLogger::init() {
    std::call_once(dbInitFlag, &PostgresThreadedLogger::initConnection, this);
    loggingThread = std::thread(&PostgresThreadedLogger::loggingThreadFunc, this);
}

void PostgresThreadedLogger::initConnection() {
    conn = PQconnectdb("host=localhost user=trader password=traderpass dbname=exchange");
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Failed to connect to database: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        conn = nullptr;
    }
}

void PostgresThreadedLogger::close() {
    running = false;
    cv.notify_all();
    if (loggingThread.joinable()) loggingThread.join();
    if (conn) {
        PQfinish(conn);
        conn = nullptr;
    }
}

void PostgresThreadedLogger::logOrder(const std::shared_ptr<Order>& order) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        orderQueue.push(order);
    }
    cv.notify_one();
}

void PostgresThreadedLogger::logCancel(OrderId canceledOrderId, long timestamp) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        cancelQueue.emplace(canceledOrderId, timestamp);
    }
    cv.notify_one();
}

void PostgresThreadedLogger::loggingThreadFunc() {
    std::vector<std::shared_ptr<Order>> orderBatch;
    std::vector<std::pair<OrderId, long>> cancelBatch;
    const auto flushInterval = std::chrono::milliseconds(10);
    const size_t maxBatchSize = 50000;

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

void PostgresThreadedLogger::flushOrderBatch(std::vector<std::shared_ptr<Order>>& batch) {
    if (!conn || batch.empty()) return;

    PGresult* res = PQexec(conn, "COPY incoming_orders (id, timestamp, side, order_type, price, volume) FROM STDIN");
    if (PQresultStatus(res) != PGRES_COPY_IN) {
        std::cerr << "COPY command failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return;
    }
    PQclear(res);

    for (const auto& o : batch) {
        std::ostringstream line;
        line << o->getId() << "\t"
             << o->getTimestamp() << "\t"
             << (o->getSide() == Side::BUY ? "BUY" : "SELL") << "\t"
             << (o->getOrderType() == OrderType::LIMIT ? "LIMIT" : "MARKET") << "\t"
             << o->getPrice() << "\t"
             << o->getOriginalVolume() << "\n";

        const std::string row = line.str();
        if (PQputCopyData(conn, row.c_str(), row.size()) != 1) {
            std::cerr << "COPY data send failed: " << PQerrorMessage(conn) << std::endl;
        }
    }

    if (PQputCopyEnd(conn, nullptr) != 1) {
        std::cerr << "COPY end failed: " << PQerrorMessage(conn) << std::endl;
    }

    PGresult* finalRes = PQgetResult(conn);
    if (PQresultStatus(finalRes) != PGRES_COMMAND_OK) {
        std::cerr << "Final COPY result error: " << PQerrorMessage(conn) << std::endl;
    }
    PQclear(finalRes);
}

void PostgresThreadedLogger::flushCancelBatch(std::vector<std::pair<OrderId, long>>& batch) {
    if (!conn || batch.empty()) return;

    PGresult* res = PQexec(conn, "COPY cancels (timestamp, order_id) FROM STDIN");
    if (PQresultStatus(res) != PGRES_COPY_IN) {
        std::cerr << "COPY command for cancels failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return;
    }
    PQclear(res);

    for (const auto& cancel : batch) {
        std::ostringstream line;
        line << cancel.second << "\t" << cancel.first << "\n";
        std::string row = line.str();
        if (PQputCopyData(conn, row.c_str(), row.size()) != 1) {
            std::cerr << "COPY cancel row failed: " << PQerrorMessage(conn) << std::endl;
        }
    }

    if (PQputCopyEnd(conn, nullptr) != 1) {
        std::cerr << "COPY end for cancels failed: " << PQerrorMessage(conn) << std::endl;
    }

    PGresult* finalRes = PQgetResult(conn);
    if (PQresultStatus(finalRes) != PGRES_COMMAND_OK) {
        std::cerr << "Final COPY result for cancels failed: " << PQerrorMessage(conn) << std::endl;
    }
    PQclear(finalRes);
}
