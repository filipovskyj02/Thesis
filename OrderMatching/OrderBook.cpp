//
// Created by JFili on 28.01.2025.
//
#include "OrderBook.h"

#include <iostream>

void OrderBook::placeOrder(const std::shared_ptr<Order>& order) {
    storeOrder(order);
    if (order->getSide() == SELL) {
        executeSell(order);
    }
    else if (order->getSide() == BUY) {
        executeBuy(order);
    }
    else {
        throw std::logic_error("Invalid order type");
    }
}

void OrderBook::executeSell(const std::shared_ptr<Order>& order) {
    while (!bids.empty() && order->getRemainingVolume() > 0) {
        while (!bids.empty() && bids.top()->isCanceled()) {
            bids.pop();
        }
        if (bids.empty()) break;;
        std::shared_ptr<Order> topBid = bids.top();

        if (order->getOrderType() == LIMIT && topBid->getPrice() < order->getPrice()) {
            break;
        }
        Volume matchedVolume = std::min(order->getRemainingVolume(), topBid->getRemainingVolume());
        order->setFilledVolume(order->getFilledVolume() + matchedVolume);
        topBid->setFilledVolume(topBid->getFilledVolume() + matchedVolume);
        lastPrice = topBid->getPrice();
        std::cout << lastPrice << '\n';
        if (topBid->getRemainingVolume() == 0) {
            bids.pop();
        }
    }

    if (order->getOrderType() == LIMIT && order->getRemainingVolume() > 0) {
        asks.push(order);
    }
}

void OrderBook::executeBuy(const std::shared_ptr<Order>& order) {
    while (!asks.empty() && order->getRemainingVolume() > 0) {
        while (!asks.empty() && asks.top()->isCanceled()) {
            asks.pop();
        }
        if (asks.empty()) break;
        std::shared_ptr<Order> topAsk = asks.top();

        if (order->getOrderType() == LIMIT && topAsk->getPrice() > order->getPrice()) {
            break;
        }
        Volume matchedVolume = std::min(order->getRemainingVolume(), topAsk->getRemainingVolume());
        order->setFilledVolume(order->getFilledVolume() + matchedVolume);
        topAsk->setFilledVolume(topAsk->getFilledVolume() + matchedVolume);
        lastPrice = topAsk->getPrice();
        std::cout << lastPrice << '\n';
        if (topAsk->getRemainingVolume() == 0) {
            asks.pop();
        }
    }
    if (order->getOrderType() == LIMIT && order->getRemainingVolume() > 0) {
        bids.push(order);
    }
}

void OrderBook::storeOrder(std::shared_ptr<Order> order) {
    OrderId orderId = order->getId();
    if (orderId >= orders.size()) {
        orders.resize(orders.size() * 2 + 1);
    }
    orders[orderId] = std::move(order);
    orderCount++;
}

void OrderBook::cancelOrder(const std::shared_ptr<Order>& order) {
    if (order->getRemainingVolume() == 0) {
        canceledFullfiledOrderCount++;
        return;
    }
    canceledOrderCount++;

    if (order->getSide() == BUY) {
        std::priority_queue<std::shared_ptr<Order>, std::vector<std::shared_ptr<Order>>, OrderCompare> newBids;
        while (!bids.empty()) {
            auto topOrder = bids.top();
            bids.pop();
            if (topOrder->getId() != order->getId()) {
                newBids.push(topOrder);
            }
        }
        bids = std::move(newBids);
    }
    else if (order->getSide() == SELL) {
        std::priority_queue<std::shared_ptr<Order>, std::vector<std::shared_ptr<Order>>, OrderCompareReverse> newAsks;
        while (!asks.empty()) {
            auto topOrder = asks.top();
            asks.pop();
            if (topOrder->getId() != order->getId()) {
                newAsks.push(topOrder);
            }
        }
        asks = std::move(newAsks);
    }
}

void OrderBook::cancelOrderLazy(const std::shared_ptr<Order>& order) {
    order->setCanceled(true);
}
std::shared_ptr<Order>& OrderBook::getOrder(OrderId id) {
    if (id >= orders.size()) {throw std::out_of_range("Invalid order id");}
    return orders[id];
}


