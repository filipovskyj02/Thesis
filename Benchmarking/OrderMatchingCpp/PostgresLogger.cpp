//
// Created by kuba on 4/5/25.
//
#include "PostgresLogger.h"
#include <iostream>
#include <sstream>

void PostgresLogger::init() {
    std::call_once(dbInitFlag, &PostgresLogger::initConnection, this);
}

void PostgresLogger::initConnection() {
    conn = PQconnectdb("host=localhost user=trader password=traderpass dbname=exchange");

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Failed to connect to database: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        conn = nullptr;
    }
}

void PostgresLogger::close() {
    if (conn) {
        PQfinish(conn);
        conn = nullptr;
    }
}

void PostgresLogger::logOrder(const std::shared_ptr<Order>& order) {
    if (!conn) return;

    std::ostringstream query;
    query << "INSERT INTO incoming_orders (id, timestamp, side, order_type, price, volume) VALUES ("
          << order->getId() << ", "
          << order->getTimestamp() << ", "
          << "'" << (order->getSide() == Side::BUY ? "BUY" : "SELL") << "', "
          << "'" << (order->getOrderType() == OrderType::LIMIT ? "LIMIT" : "MARKET") << "', "
          << order->getPrice() << ", "
          << order->getOriginalVolume() << ");";

    PGresult* res = PQexec(conn, query.str().c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Insert failed: " << PQerrorMessage(conn) << std::endl;
    }

    PQclear(res);
}

void PostgresLogger::logCancel(OrderId canceledOrderId, long timestamp) {
    if (!conn) return;

    std::ostringstream query;
    query << "INSERT INTO cancels (timestamp, order_id) VALUES ("
          << timestamp << ", " << canceledOrderId << ");";

    PGresult* res = PQexec(conn, query.str().c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Cancel insert failed: " << PQerrorMessage(conn) << std::endl;
    }

    PQclear(res);
}
