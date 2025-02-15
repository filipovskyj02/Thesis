//
// Created by JFili on 28.01.2025.
//

#ifndef ORDERBOOK_H
#define ORDERBOOK_H
#include <queue>

#include "Order.h"




class OrderBook {
public:
    void placeOrder(const std::shared_ptr<Order>& order);
    void cancelOrder(const std::shared_ptr<Order>& order);
    void cancelOrderLazy(const std::shared_ptr<Order>& order);
    std::shared_ptr<Order>& getOrder(OrderId id);
    OrderId orderCount = 0;
    int canceledFullfiledOrderCount = 0;
    int canceledOrderCount = 0;

private:
    struct OrderCompare {
        bool operator()(const std::shared_ptr<Order>& lhs, const std::shared_ptr<Order>& rhs) const {
            return *lhs < *rhs;
        }
    };
    // default max heap - makes sense for buys
    std::priority_queue<std::shared_ptr<Order>, std::vector<std::shared_ptr<Order>>, OrderCompare> bids;
    std::priority_queue<std::shared_ptr<Order>, std::vector<std::shared_ptr<Order>>, std::greater<>> asks;
    std::vector<std::shared_ptr<Order>> orders;
    void storeOrder(std::shared_ptr<Order> order);
    void executeSell(const std::shared_ptr<Order>& order);
    void executeBuy(const std::shared_ptr<Order>& order);


};
#endif //ORDERBOOK_H
