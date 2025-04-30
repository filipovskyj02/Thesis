#include "OrderBook.h"


bool OrderBook::placeOrder(const std::shared_ptr<Order>& order) {
    storeOrder(order);
    if (order->getSide() == SELL) {
        return executeSell(order);
    }
    if (order->getSide() == BUY) {
        return executeBuy(order);
    }
    throw std::logic_error("Invalid order type");
}

bool OrderBook::executeSell(const std::shared_ptr<Order>& order) {
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

        aggregatedBids.at(topBid->getPrice()) -= matchedVolume;
        lastPrice = topBid->getPrice();
        if (topBid->getRemainingVolume() == 0) {
            bids.pop();
        }
    }

    if (order->getOrderType() == LIMIT && order->getRemainingVolume() > 0) {
        asks.push(order);
        if (!aggregatedAsks.contains(order->getPrice())) {
            aggregatedAsks[order->getPrice()] = order->getRemainingVolume();
        }
        else aggregatedAsks[order->getPrice()] += order->getRemainingVolume();
        return true;
    }
    // Not enough liquidity to fulfill Market order
    if (order->getOrderType() == MARKET && order->getRemainingVolume() > 0) {
        return false;
    }
    return true;
}

bool OrderBook::executeBuy(const std::shared_ptr<Order>& order) {
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

        aggregatedAsks[topAsk->getPrice()] -= matchedVolume;
        lastPrice = topAsk->getPrice();
        if (topAsk->getRemainingVolume() == 0) {
            asks.pop();
        }
    }
    if (order->getOrderType() == LIMIT && order->getRemainingVolume() > 0) {
        bids.push(order);
        if (!aggregatedBids.contains(order->getPrice())) {
            aggregatedBids[order->getPrice()] = order->getRemainingVolume();
        }
        else aggregatedBids[order->getPrice()] += order->getRemainingVolume();
        return true;
    }
    // Not enough liquidity to fulfill Market order
    if (order->getOrderType() == MARKET && order->getRemainingVolume() > 0) {
        return false;
    }
    return true;
}

void OrderBook::storeOrder(std::shared_ptr<Order> order) {
    orders.push_back(order);
    orderCount++;
}

bool OrderBook::cancelOrderLazy(const std::shared_ptr<Order>& order) {
    if (order->getOrderType() == MARKET || order->getRemainingVolume() == 0) return false;
    order->setCanceled(true);
    return true;
}
std::shared_ptr<Order>& OrderBook::getOrder(OrderId id) {
    if (id >= orders.size()) {throw std::out_of_range("Invalid order id");}
    return orders[id];
}