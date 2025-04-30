#pragma once

#include <chrono>

#include "SideEnum.h"
#include "CommonTypes.h"
#include "OrderType.h"

class Order {
public:
    Order(Side side, OrderType orderType, Price price, Volume volume)
       : id(generateId()), side(side), orderType(orderType), price(price), volume(volume) {
        timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count();
        filledVolume = 0;
        canceled = false;

    }

    Order(Side side, OrderType orderType, Volume volume) : id(generateId()), side(side), orderType(orderType), volume(volume) {
        timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
                       std::chrono::system_clock::now().time_since_epoch()).count();
        price = 0;
        filledVolume = 0;
        canceled = false;
    }
    int getId() const { return id; }
    OrderType getOrderType() const { return orderType; }
    Side getSide() const { return side; }

    double getPrice() const { return price; }
    void setPrice(double newPrice) { price = newPrice; }

    Volume getFilledVolume() const { return filledVolume; }
    void setFilledVolume(Volume newVolume) { filledVolume = newVolume; }

    Volume getRemainingVolume() const { return volume - filledVolume; }

    Volume getOriginalVolume() const { return volume ; }

    long getTimestamp() const { return timestamp; }
    void setTimestamp(long newTimestamp) { timestamp = newTimestamp; }

    bool isCanceled() const { return canceled; }
    void setCanceled(const bool newCanceled) { canceled = newCanceled; }



    bool operator<(const Order& other) const {
        if (this->price == other.price) {
            return this->timestamp > other.timestamp;
        }
        return this->price < other.price;
    }
    friend std::ostream& operator<<(std::ostream& os, const Order& order) {
        return os << order.id << " " << order.side << " " << order.price << " " << order.volume << " " << order.timestamp << " filled volume: " << order.filledVolume  << '\n';
    }
private:
    const OrderId id;
    const Side side;
    const OrderType orderType;
    Price price;
    Volume volume;
    Volume filledVolume;
    long long timestamp;
    bool canceled;


    static int generateId() {
        static int currentId = 0;
        return currentId++;
    }
};
#ifndef ORDER_H
#define ORDER_H

#endif //ORDER_H
