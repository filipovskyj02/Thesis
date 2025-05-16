#include "OrderBook.h"
#include <iostream>

#include <boost/smart_ptr/shared_ptr.hpp>

OrderBook::OrderBook(const std::string ticker,
                     SafeQueue<std::shared_ptr<Order>>& inQueue,
                     SafeQueue<DisseminationEvent> &disseminationQueue,
                     SafeQueue<LogEvent> &logQueue,
                     SafeQueue<NotificationEvent> &notificationQueue)
        :   ticker(ticker),
            inQueue(inQueue),
            disseminationQueue(disseminationQueue),
            logQueue(logQueue),
            notificationQueue(notificationQueue)
{}

void OrderBook::run() {
    while (true) {
        auto optOrder = inQueue.pop();
        if (!optOrder) {
            break;
        }

        auto& order = optOrder.value();
        placeOrder(order);
    }
}

bool OrderBook::placeOrder(const std::shared_ptr<Order>& order) {
    storeOrder(order);
    emitPlacedNotification(order);
    touchedLevels.clear();
    bool result;
    if (order->getOrderType() == CANCEL) result = cancelOrderLazy(order);
    else if (order->getSide() == SELL)      result = executeSell(order);
    else if (order->getSide() == BUY)  result = executeBuy(order);
    else                                throw std::logic_error("Invalid order type");
    emitLevel2UpdateBatch(order);
    return result;
}

bool OrderBook::executeSell(const std::shared_ptr<Order>& order) {
    while (!bids.empty() && order->getRemainingVolume() > 0) {
        while (!bids.empty() && bids.top()->isCanceled()) {
            bids.pop();
        }
        if (bids.empty()) break;
        std::shared_ptr<Order> topBid = bids.top();

        if (order->getOrderType() == LIMIT && topBid->getPrice() < order->getPrice()) {
            break;
        }
        touchLevel(topBid->getPrice());
        Volume matchedVolume = std::min(order->getRemainingVolume(), topBid->getRemainingVolume());
        aggregatedBids.at(topBid->getPrice()) -= matchedVolume;
        lastPrice = topBid->getPrice();

        order->setFilledVolume(order->getFilledVolume() + matchedVolume);
        topBid->setFilledVolume(topBid->getFilledVolume() + matchedVolume);

        afterMatch(matchedVolume);
        logEvent(order);
        if (order->getRemainingVolume() > 0) emitPartialNotification(order, matchedVolume);
        else emitFilledNotification(order);

        if (topBid->getRemainingVolume() == 0) {
            bids.pop();
        }
    }

    if (order->getOrderType() == LIMIT && order->getRemainingVolume() > 0) {
        asks.push(order);
        restLimitAsk(order->getPrice(), order->getRemainingVolume());
        logEvent(order);
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
        touchLevel(topAsk->getPrice());
        Volume matchedVolume = std::min(order->getRemainingVolume(), topAsk->getRemainingVolume());
        lastPrice = topAsk->getPrice();
        aggregatedAsks[topAsk->getPrice()] -= matchedVolume;
        order->setFilledVolume(order->getFilledVolume() + matchedVolume);
        topAsk->setFilledVolume(topAsk->getFilledVolume() + matchedVolume);

        afterMatch(matchedVolume);
        logEvent(order);
        if (order->getRemainingVolume() > 0) emitPartialNotification(order, matchedVolume);
        else emitFilledNotification(order);

        if (topAsk->getRemainingVolume() == 0) {
            asks.pop();
        }
    }
    if (order->getOrderType() == LIMIT && order->getRemainingVolume() > 0) {
        bids.push(order);
        restLimitBuy(order->getPrice(), order->getRemainingVolume());
        logEvent(order);
        return true;
    }
    // Not enough liquidity to fulfill Market order
    if (order->getOrderType() == MARKET && order->getRemainingVolume() > 0) {
        return false;
    }
    return true;
}


bool OrderBook::cancelOrderLazy(const std::shared_ptr<Order>& order) {
    if (order->getCancelTarget().empty() or !orders.contains(order->getCancelTarget())) {
        return false;
    }
    auto targetOrder = orders.at(order->getCancelTarget());
    if (targetOrder->getOrderType() == MARKET || targetOrder->getRemainingVolume() == 0) {
        return false;
    }

    targetOrder->setCanceled(true);

    if (targetOrder->getSide() == BUY) aggregatedBids[targetOrder->getPrice()] -= targetOrder->getRemainingVolume();
    if (targetOrder->getSide() == SELL) aggregatedAsks[targetOrder->getPrice()] -= targetOrder->getRemainingVolume();

    emitCanceledNotification(targetOrder);
    return true;
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
    Volume bestBidV = bids.empty() ? 0             : aggregatedBids.at(bids.top()->getPrice());
    Price bestAskP = asks.empty() ? Price(-1) : asks.top()->getPrice();
    Volume bestAskV = asks.empty() ? 0             : aggregatedAsks.at(asks.top()->getPrice());

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
void OrderBook::emitLevel2UpdateBatch(const std::shared_ptr<Order>& order) {
    if (touchedLevels.empty()) return;

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
        .timestamp = std::chrono::system_clock::now(),
        .ingestionTimestamp = order->getTimestamp(),
        .orderCreationTimestamp = order->getCreationTimestamp()
    };
    disseminationQueue.push(DisseminationEvent{u});
}

void OrderBook::logEvent(const std::shared_ptr<Order>& order) {
    logQueue.push(LogEvent{
        ticker,
        order,
        std::chrono::system_clock::now()
   });
}

void OrderBook::emitPlacedNotification(const std::shared_ptr<Order>& order) {
    NotificationEvent evt = OrderPlaced{
        .orderId         = order->getId(),
        .userId          = order->getUserId(),
        .ticker          = ticker,
        .limitPrice      = order->getOrderType() == LIMIT
                              ? order->getPrice()
                              : 0,
        .originalVolume  = order->getOriginalVolume(),
        .timestamp       = std::chrono::system_clock::now()
    };
    notificationQueue.push(std::move(evt));
}

void OrderBook::emitPartialNotification(const std::shared_ptr<Order>& order,
                                        Volume matchedVolume)
{
    NotificationEvent evt = OrderPartialFill{
        .orderId          = order->getId(),
        .userId           = order->getUserId(),
        .ticker           = ticker,
        .execPrice        = lastPrice,
        .filledVolume     = matchedVolume,
        .cumulativeFilled = order->getFilledVolume(),
        .remaining        = order->getRemainingVolume(),
        .timestamp        = std::chrono::system_clock::now()
    };
    notificationQueue.push(std::move(evt));
}

void OrderBook::emitFilledNotification(const std::shared_ptr<Order>& order) {
    NotificationEvent evt = OrderFilled{
        .orderId      = order->getId(),
        .userId       = order->getUserId(),
        .ticker       = ticker,
        .execPrice    = lastPrice,
        .totalVolume  = order->getOriginalVolume(),
        .timestamp    = std::chrono::system_clock::now()
    };
    notificationQueue.push(std::move(evt));
}

void OrderBook::emitCanceledNotification(const std::shared_ptr<Order>& targetOrder) {
    NotificationEvent evt = OrderCanceled{
        .orderId           = targetOrder->getId(),
        .userId            = targetOrder->getUserId(),
        .ticker            = ticker,
        .canceledRemaining = targetOrder->getRemainingVolume(),
        .timestamp         = std::chrono::system_clock::now()
    };
    notificationQueue.push(std::move(evt));
}

