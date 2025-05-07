#pragma once

#include <chrono>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "SideEnum.h"
#include "CommonTypes.h"
#include "OrderType.h"
inline const char* toString(Side s) noexcept {
    return s == BUY  ? "BUY"
         : s == SELL ? "SELL"
         :             "UNKNOWN";
}

inline const char* toString(OrderType t) noexcept {
    return t == MARKET ? "MARKET"
         : t == LIMIT  ? "LIMIT"
         :               "UNKNOWN";
}

class Order {
    const std::string id;
    const UserId userId;
    long long timestamp;
    long long creationTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    const Side side;
    const OrderType orderType;
    Price price;
    Volume volume;
    const std::string cancelTarget;

    Volume filledVolume = 0;
    bool canceled = false;
public:
    // Limit
    Order(std::string id,
         UserId userId,
         long long timestamp,
         Side side,
         OrderType orderType,
         Price price,
         Volume volume)
     : id(std::move(id))
     , userId(userId)
     , timestamp(timestamp)
     , side(side)
     , orderType(orderType)
     , price(price)
     , volume(volume) {}

    // Market
    Order(std::string id,
          UserId userId,
          long long timestamp,
          Side side,
          OrderType orderType,
          Volume volume)
      : id(std::move(id))
      , userId(userId)
      , timestamp(timestamp)
      , side(side)
      , orderType(orderType)
      , price(0)
      , volume(volume)
    {}

    // 3) Cancel
    Order(std::string id,
          UserId userId,
          long long timestamp,
          std::string cancelTargetOrderId)
      : id(std::move(id))
      , userId(userId)
      , timestamp(timestamp)
      , side(Side::BUY)
      , orderType(OrderType::CANCEL)
      , price(0)
      , volume(0)
      , cancelTarget(std::move(cancelTargetOrderId))
    {}

    // 4) fully explicit
    Order(std::string id,
          UserId userId,
          long long timestamp,
          Side side,
          OrderType orderType,
          Price price,
          Volume volume,
          std::string cancelTargetOrderId)
      : id(std::move(id))
      , userId(userId)
      , timestamp(timestamp)
      , side(side)
      , orderType(orderType)
      , price(price)
      , volume(volume)
      , cancelTarget(std::move(cancelTargetOrderId))
    {}

    //─────────────────────────────────────────────────────────────────────────
    // For tests
    Order(Side side,
          OrderType type,
          Price price,
          Volume volume)
      : id(generateId())
      , userId(0)
      , timestamp(0)
      , side(side)
      , orderType(type)
      , price(price)
      , volume(volume)
    {}

    Order(Side side,
          OrderType type,
          Volume volume)
      : id(generateId())
      , userId(0)
      , timestamp(0)
      , side(side)
      , orderType(type)
      , price(0)
      , volume(volume)
    {}

    Order(std::string targetId)
      : id(generateId())
      , userId(0)
      , timestamp(0)
      , side(BUY)
      , orderType(CANCEL)
      , price(0)
      , volume(0)
      , cancelTarget(targetId)
    {}
    std::string getId() const { return id; }
    UserId getUserId() const { return userId; }
    Side getSide() const { return side; }
    OrderType getOrderType() const { return orderType; }
    Price getPrice() const { return price; }
    long long getTimestamp() const { return timestamp; }
    const std::string& getCancelTarget() const { return cancelTarget; }

    Volume getFilledVolume() const { return filledVolume; }
    void setFilledVolume(Volume newVolume) { filledVolume = newVolume; }
    Volume getRemainingVolume() const { return volume - filledVolume; }
    Volume getOriginalVolume() const { return volume ; }
    bool isCanceled() const { return canceled; }
    void setCanceled(const bool newCanceled) { canceled = newCanceled; }
    long long getCreationTimestamp() const { return creationTimestamp; }

    bool operator<(const Order& other) const {
        if (this->price == other.price) {
            return this->timestamp > other.timestamp;
        }
        return this->price < other.price;
    }

    friend std::ostream& operator<<(std::ostream& os, const Order& o) {
        os
          << "id="        << o.id
          << " side="     << toString(o.side)
          << " type="     << toString(o.orderType)
          << " price="    << o.price
          << " volume="   << o.volume
          << " ts="       << o.timestamp
          << " filled="   << o.filledVolume;
        return os;
    }
private:

    static std::string generateId() {
        return to_string(boost::uuids::random_generator()());
    }

};
#ifndef ORDER_H
#define ORDER_H

#endif //ORDER_H
