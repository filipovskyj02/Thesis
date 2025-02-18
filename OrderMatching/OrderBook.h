//
// Created by JFili on 28.01.2025.
//

#ifndef ORDERBOOK_H
#define ORDERBOOK_H
#include <iostream>
#include <queue>

#include "Order.h"




class OrderBook {
public:
    void placeOrder(const std::shared_ptr<Order>& order);
    void cancelOrder(const std::shared_ptr<Order>& order);
    void cancelOrderLazy(const std::shared_ptr<Order>& order);
    void countFilledCount();
    std::shared_ptr<Order>& getOrder(OrderId id);
    OrderId orderCount = 0;
    int canceledFullfiledOrderCount = 0;
    int canceledOrderCount = 0;
    Price lastPrice = 0;

private:
    struct OrderCompare {
        bool operator()(const std::shared_ptr<Order>& lhs, const std::shared_ptr<Order>& rhs) const {
            return *lhs < *rhs;
        }
    };
    struct OrderCompareReverse {
        bool operator()(const std::shared_ptr<Order>& lhs, const std::shared_ptr<Order>& rhs) const {
            return !(*lhs < *rhs);
        }
    };
    // default max heap - makes sense for buys
    std::priority_queue<std::shared_ptr<Order>, std::vector<std::shared_ptr<Order>>, OrderCompare> bids;
    std::priority_queue<std::shared_ptr<Order>, std::vector<std::shared_ptr<Order>>, OrderCompareReverse> asks;
    std::vector<std::shared_ptr<Order>> orders;
    void storeOrder(std::shared_ptr<Order> order);
    void executeSell(const std::shared_ptr<Order>& order);
    void executeBuy(const std::shared_ptr<Order>& order);


};

inline void OrderBook::countFilledCount() {
    uint64_t count = 0;
    uint64_t totalVolume = 0;
    for (int i = 0; i < orderCount; i++) {
        if (orders[i] != nullptr && orders[i]->getFilledVolume() == orders[i]->getOriginalVolume()) {
            totalVolume+= orders[i]->getFilledVolume();
            count++;
        }
    }
    std::cout << " Total filled volume: " << totalVolume <<  " by " <<  count << " orders" << std::endl;
}
#endif //ORDERBOOK_H
