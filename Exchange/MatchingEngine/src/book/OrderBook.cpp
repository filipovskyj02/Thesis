#include "OrderBook.h"
#include <iostream>

OrderBook::OrderBook(const std::string& ticker,
                     SafeQueue<std::shared_ptr<Order>>& inQueue,
                     SafeQueue<DisseminationEvent> &disseminationQueue)
        :   ticker(ticker),
            inQueue(inQueue),
            disseminationQueue(disseminationQueue) {}

void OrderBook::run() {
    while (true) {
        auto optOrder = inQueue.pop();
        if (!optOrder) break;

        auto& order = optOrder.value();
        placeOrder(order);
    }
}

bool OrderBook::placeOrder(const std::shared_ptr<Order>& order) {
    storeOrder(order);
    touchedLevels.clear();
    bool result;
    if (order->getSide() == SELL)      result = executeSell(order);
    else if (order->getSide() == BUY)  result = executeBuy(order);
    else                                throw std::logic_error("Invalid order type");
    emitLevel2UpdateBatch();
    return result;

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
        { Price lvl = topBid->getPrice();
                      if (touchedLevels.empty() || touchedLevels.back() != lvl)
                             touchedLevels.push_back(lvl);
                    }
        Volume matchedVolume = std::min(order->getRemainingVolume(), topBid->getRemainingVolume());
        aggregatedBids.at(topBid->getPrice()) -= matchedVolume;
        lastPrice = topBid->getPrice();

        order->setFilledVolume(order->getFilledVolume() + matchedVolume);
        topBid->setFilledVolume(topBid->getFilledVolume() + matchedVolume);

        emitTradeEvent(matchedVolume);
        maybeEmitLevel1();

        if (topBid->getRemainingVolume() == 0) {
            bids.pop();
        }
    }

    if (order->getOrderType() == LIMIT && order->getRemainingVolume() > 0) {
        asks.push(order);
        {
            Price p = order->getPrice();
            touchedLevels.push_back(p);
        }

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
        { Price lvl = topAsk->getPrice();
                      if (touchedLevels.empty() || touchedLevels.back() != lvl)
                            touchedLevels.push_back(lvl);
                    }
        Volume matchedVolume = std::min(order->getRemainingVolume(), topAsk->getRemainingVolume());
        lastPrice = topAsk->getPrice();
        aggregatedAsks[topAsk->getPrice()] -= matchedVolume;
        order->setFilledVolume(order->getFilledVolume() + matchedVolume);
        topAsk->setFilledVolume(topAsk->getFilledVolume() + matchedVolume);

        emitTradeEvent(matchedVolume);
        maybeEmitLevel1();

        if (topAsk->getRemainingVolume() == 0) {
            asks.pop();
        }
    }
    if (order->getOrderType() == LIMIT && order->getRemainingVolume() > 0) {
        bids.push(order);
        {
            Price p = order->getPrice();
            touchedLevels.push_back(p);
        }
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


bool OrderBook::cancelOrderLazy(const std::shared_ptr<Order>& order) {
    if (order->getOrderType() == MARKET || order->getRemainingVolume() == 0) return false;
    order->setCanceled(true);
    return true;
}
std::shared_ptr<Order>& OrderBook::getOrder(OrderId id) {
    if (id >= orders.size()) {throw std::out_of_range("Invalid order id");}
    return orders[id];
}

void OrderBook::emitTradeEvent(Volume matchedVolume) {
    TradeEvent te{
        .ticker    = ticker,
        .price     = lastPrice,
        .volume    = matchedVolume,
        .timestamp = std::chrono::system_clock::now()
    };
    disseminationQueue.push(DisseminationEvent{te});
}

void OrderBook::maybeEmitLevel1() {
    Price bestBidP = bids.empty() ? Price(-1) : bids.top()->getPrice();
    Volume bestBidV = bids.empty() ? 0             : bids.top()->getRemainingVolume();
    Price bestAskP = asks.empty() ? Price(-1) : asks.top()->getPrice();
    Volume bestAskV = asks.empty() ? 0             : asks.top()->getRemainingVolume();

    if (bestBidP!=lastBestBidPrice ||
        bestBidV!=lastBestBidSize  ||
        bestAskP!=lastBestAskPrice ||
        bestAskV!=lastBestAskSize)
    {
        lastBestBidPrice = bestBidP;
        lastBestBidSize  = bestBidV;
        lastBestAskPrice = bestAskP;
        lastBestAskSize  = bestAskV;

        Level1Update u{
            .ticker        = ticker,
            .bestBidPrice  = bestBidP,
            .bestBidSize   = bestBidV,
            .bestAskPrice  = bestAskP,
            .bestAskSize   = bestAskV,
            .timestamp     = std::chrono::system_clock::now()
        };
        disseminationQueue.push(DisseminationEvent{u});
    }
}
void OrderBook::emitLevel2UpdateBatch() {
    if (touchedLevels.empty()) return;

    // Build one L2 message with all changed levels
    std::vector<std::pair<Price,Volume>> depth;
    depth.reserve(touchedLevels.size());
    for (Price p : touchedLevels) {
        Volume v = 0;
        if (aggregatedBids.contains(p)) v = aggregatedBids[p];
        else if (aggregatedAsks.contains(p)) v = aggregatedAsks[p];
        depth.emplace_back(p, v);
    }

    Level2Update u{
        .ticker    = ticker,
        .depth     = std::move(depth),
        .timestamp = std::chrono::system_clock::now()
    };
    disseminationQueue.push(DisseminationEvent{u});
}
