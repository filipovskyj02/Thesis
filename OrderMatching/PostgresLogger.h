//
// Created by kuba on 4/5/25.
//
#ifndef POSTGRESLOGGER_H
#define POSTGRESLOGGER_H

#include <memory>
#include <mutex>
#include <libpq-fe.h>
#include "Order.h"

class PostgresLogger {
public:
    void init();
    void close();
    void logOrder(const std::shared_ptr<Order>& order);
    void logCancel(OrderId canceledOrderId, long timestamp);

private:
    PGconn* conn = nullptr;
    std::once_flag dbInitFlag;

    void initConnection();
};


#endif //POSTGRESLOGGER_H
