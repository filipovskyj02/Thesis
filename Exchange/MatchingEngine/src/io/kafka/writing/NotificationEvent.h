#ifndef NOTIFICATIONEVENT_H
#define NOTIFICATIONEVENT_H
#include <chrono>
#include <string>
#include <variant>

#include "../../../common/CommonTypes.h"

struct OrderPlaced {
    std::string   orderId;
    UserId       userId;
    std::string   ticker;
    Price        limitPrice;
    Volume       originalVolume;
    std::chrono::system_clock::time_point timestamp;
};

struct OrderPartialFill {
    std::string   orderId;
    UserId       userId;
    std::string   ticker;
    Price        execPrice;
    Volume       filledVolume;
    Volume       cumulativeFilled;
    Volume       remaining;
    std::chrono::system_clock::time_point timestamp;
};

struct OrderFilled {
    std::string   orderId;
    UserId       userId;
    std::string   ticker;
    Price        execPrice;
    Volume       totalVolume;
    std::chrono::system_clock::time_point timestamp;
};

struct OrderCanceled {
    std::string   orderId;
    UserId       userId;
    std::string   ticker;
    Volume       canceledRemaining;
    std::chrono::system_clock::time_point timestamp;
};

struct OrderRejected {
    std::string   orderId;
    UserId       userId;
    std::string   ticker;
    std::string   reason;
    std::chrono::system_clock::time_point timestamp;
};


using NotificationEvent = std::variant<
    OrderPlaced,
    OrderPartialFill,
    OrderFilled,
    OrderCanceled,
    OrderRejected
>;



inline std::string toString(const OrderPlaced& e) {
    // PLACED,orderId,userId,ticker,limitPrice,originalVolume,timestampMs
    return std::string{"PLACED,"} +
        e.orderId + ',' +
        std::to_string(e.userId) + ',' +
        e.ticker + ',' +
        std::to_string(e.limitPrice) + ',' +
        std::to_string(e.originalVolume) + ',' +
        std::to_string(
          std::chrono::duration_cast<std::chrono::milliseconds>(
            e.timestamp.time_since_epoch()
          ).count()
        );
}

inline std::string toString(const OrderPartialFill& e) {
    // PARTIAL,orderId,userId,ticker,execPrice,filledVol,cumFilled,remaining,timestampMs
    return std::string{"PARTIAL,"} +
        e.orderId + ',' +
        std::to_string(e.userId) + ',' +
        e.ticker + ',' +
        std::to_string(e.execPrice) + ',' +
        std::to_string(e.filledVolume) + ',' +
        std::to_string(e.cumulativeFilled) + ',' +
        std::to_string(e.remaining) + ',' +
        std::to_string(
          std::chrono::duration_cast<std::chrono::milliseconds>(
            e.timestamp.time_since_epoch()
          ).count()
        );
}

inline std::string toString(const OrderFilled& e) {
    // FILLED,orderId,userId,ticker,execPrice,totalVol,timestampMs
    return std::string{"FILLED,"} +
        e.orderId + ',' +
        std::to_string(e.userId) + ',' +
        e.ticker + ',' +
        std::to_string(e.execPrice) + ',' +
        std::to_string(e.totalVolume) + ',' +
        std::to_string(
          std::chrono::duration_cast<std::chrono::milliseconds>(
            e.timestamp.time_since_epoch()
          ).count()
        );
}

inline std::string toString(const OrderCanceled& e) {
    // CANCELED,orderId,userId,ticker,canceledRem,timestampMs
    return std::string{"CANCELED,"} +
        e.orderId + ',' +
        std::to_string(e.userId) + ',' +
        e.ticker + ',' +
        std::to_string(e.canceledRemaining) + ',' +
        std::to_string(
          std::chrono::duration_cast<std::chrono::milliseconds>(
            e.timestamp.time_since_epoch()
          ).count()
        );
}

inline std::string toString(const OrderRejected& e) {
    // REJECTED,orderId,userId,ticker,reasonCode,timestampMs
    return std::string{"REJECTED,"} +
        e.orderId + ',' +
        std::to_string(e.userId) + ',' +
        e.ticker + ',' +
        e.reason + ',' +
        std::to_string(
          std::chrono::duration_cast<std::chrono::milliseconds>(
            e.timestamp.time_since_epoch()
          ).count()
        );
}

inline std::string toString(const NotificationEvent& evt) {
    return std::visit([](auto const& e){
        return toString(e) + '\n';
    }, evt);
}

#endif //NOTIFICATIONEVENT_H
