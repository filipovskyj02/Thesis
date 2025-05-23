#ifndef DISSEMINATIONEVENT_H
#define DISSEMINATIONEVENT_H
#include <chrono>
#include <vector>
#include <variant>

#include "../../common/CommonTypes.h"
#include "../../common/Utils.h"

struct TradeEvent {
    std::string   ticker;
    Price        price;
    Volume      volume;
    std::chrono::system_clock::time_point timestamp;
};

struct Level1Update {
    std::string   ticker;
    Price        bestBidPrice;
    Volume      bestBidSize;
    Price        bestAskPrice;
    Volume      bestAskSize;
    std::chrono::system_clock::time_point timestamp;
};

struct Level2Update {
    std::string   ticker;
    std::vector<std::pair<Price,Volume>> depth;
    std::chrono::system_clock::time_point timestamp;
    long long ingestionTimestamp;
    long long orderCreationTimestamp;
};

using DisseminationEvent = std::variant<
    TradeEvent,
    Level1Update,
    Level2Update
>;

inline std::string toString(const TradeEvent& e) {
    std::ostringstream ss;
    ss << "TRADE,"
       << e.ticker << ','
       << e.price  << ','
       << e.volume << ','
       << formatTimestamp(e.timestamp);
    return ss.str();
}

inline std::string toString(const Level1Update& u) {
    std::ostringstream ss;
    ss << "L1,"
       << u.ticker       << ','
       << u.bestBidPrice << ','
       << u.bestBidSize  << ','
       << u.bestAskPrice << ','
       << u.bestAskSize  << ','
       << formatTimestamp(u.timestamp);
    return ss.str();
}

inline std::string toString(const Level2Update& u) {
    std::ostringstream ss;
    ss << "L2," << u.ticker;
    for (auto& [price, vol] : u.depth) {
        ss << ',' << price << ':' << vol;
    }
    ss << ',' << std::chrono::duration_cast<
                          std::chrono::milliseconds>(
                          u.timestamp
                          .time_since_epoch()).count() << "," << u.orderCreationTimestamp << "," << u.ingestionTimestamp;
    return ss.str();
}

inline std::string toString(const DisseminationEvent& evt) {
    return std::visit([](auto&& arg){
        return toString(arg) + '\n';
    }, evt);
}

#endif //DISSEMINATIONEVENT_H
