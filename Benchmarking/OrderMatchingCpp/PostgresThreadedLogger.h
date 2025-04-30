//
// Created by kuba on 4/5/25.
//

#ifndef POSTGRESTHREADEDLOGGER_H
#define POSTGRESTHREADEDLOGGER_H

#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <libpq-fe.h>
#include "Order.h"


class PostgresThreadedLogger {
public:
    void init();
    void logOrder(const std::shared_ptr<Order>& order);
    void logCancel(OrderId canceledOrderId, long timestamp);
    void close();

private:
    void initConnection();
    void loggingThreadFunc();
    void flushOrderBatch(std::vector<std::shared_ptr<Order>>& batch);
    void flushCancelBatch(std::vector<std::pair<OrderId, long>>& batch);

    PGconn* conn = nullptr;
    std::once_flag dbInitFlag;

    std::queue<std::shared_ptr<Order>> orderQueue;
    std::queue<std::pair<OrderId, long>> cancelQueue;
    std::mutex mutex;
    std::condition_variable cv;
    std::thread loggingThread;
    std::atomic<bool> running = true;
};



#endif //POSTGRESTHREADEDLOGGER_H
