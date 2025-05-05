#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <queue>
#include <unordered_map>

#include "../common/SafeQueue.h"
#include "../io/dissemination/DisseminationEvent.h"
#include "../common/Order.h"
#include "../io/kafka/writing/NotificationEvent.h"
#include "../io/logging/LogEvent.h"

class OrderBook {
public:
    OrderBook(const std::string ticker, SafeQueue<std::shared_ptr<Order> > &inQueue,
              SafeQueue<DisseminationEvent> &disseminationQueue,
              SafeQueue<LogEvent> &logQueue,
              SafeQueue<NotificationEvent> &notificationQueue);

    void run();

    bool placeOrder(const std::shared_ptr<Order> &order);

    bool cancelOrderLazy(const std::shared_ptr<Order> &order);

    Price lastPrice = 0;

    struct OrderCompare {
        bool operator()(const std::shared_ptr<Order> &lhs, const std::shared_ptr<Order> &rhs) const {
            return *lhs < *rhs;
        }
    };

    struct OrderCompareReverse {
        bool operator()(const std::shared_ptr<Order> &lhs, const std::shared_ptr<Order> &rhs) const {
            if (lhs->getPrice() == rhs->getPrice()) {
                return lhs->getTimestamp() > rhs->getTimestamp();
            }
            return !(*lhs < *rhs);
        }
    };

    std::priority_queue<std::shared_ptr<Order>, std::vector<std::shared_ptr<Order> >, OrderCompare> bids;
    std::priority_queue<std::shared_ptr<Order>, std::vector<std::shared_ptr<Order> >, OrderCompareReverse> asks;
    std::unordered_map<std::string, std::shared_ptr<Order>> orders;
    std::unordered_map<Price, Volume> aggregatedBids;
    std::unordered_map<Price, Volume> aggregatedAsks;

private:
    const std::string ticker;
    SafeQueue<std::shared_ptr<Order> > &inQueue;
    SafeQueue<DisseminationEvent> &disseminationQueue;
    SafeQueue<LogEvent> &logQueue;
    SafeQueue<NotificationEvent> &notificationQueue;


    void storeOrder(const std::shared_ptr<Order> &order) { orders.insert({order->getId(), order}); }

    bool executeSell(const std::shared_ptr<Order> &order);

    bool executeBuy(const std::shared_ptr<Order> &order);

    void maybeEmitLevel1();

    void emitTradeEvent(Volume matchedVolume);

    std::vector<Price> touchedLevels;

    void emitLevel2UpdateBatch();

    void logEvent(const std::shared_ptr<Order>& order);

    void emitPlacedNotification(const std::shared_ptr<Order>& order);
    void emitPartialNotification(const std::shared_ptr<Order>& order,
                                 Volume matchedVolume);
    void emitFilledNotification(const std::shared_ptr<Order>& order);
    void emitCanceledNotification(const std::shared_ptr<Order>& targetOrder);


    Price lastBestBidPrice = -1;
    Volume lastBestBidSize = 0;
    Price lastBestAskPrice = -1;
    Volume lastBestAskSize = 0;

    void touchLevel(Price p) {
        if (touchedLevels.empty() || touchedLevels.back() != p)
            touchedLevels.push_back(p);
    }

    void afterMatch(Volume matched) {
        emitTradeEvent(matched);
        maybeEmitLevel1();
    }

    void restLimitAsk(Price p, Volume vol) {
        touchLevel(p);
        aggregatedAsks[p] += vol;
    }

    void restLimitBuy(Price p, Volume vol) {
        touchLevel(p);
        aggregatedBids[p] += vol;
    }
};
#endif //ORDERBOOK_H
