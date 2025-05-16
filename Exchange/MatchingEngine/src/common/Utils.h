
#ifndef UTILS_H
#define UTILS_H


#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

#include "Order.h"

inline std::string formatTimestamp(
    const std::chrono::system_clock::time_point& tp)
{
    using namespace std::chrono;
    auto t     = system_clock::to_time_t(tp);
    auto micros = duration_cast<microseconds>(tp.time_since_epoch()) % 1000000;

    std::tm tm = *std::localtime(&t);
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S")
       << '.' << std::setw(6) << std::setfill('0') << micros.count();
    return ss.str();
}

inline std::vector<std::string> splitLine(const std::string& s) {
    std::vector<std::string> out;
    std::string part;
    std::istringstream ss(s);
    while (std::getline(ss, part, ',')) out.push_back(part);
    return out;
}

// msgType,userId,ticker,orderId,timestamp,side,price,volume,cancelId
inline std::pair<std::string,std::shared_ptr<Order>> parseRecord(const std::string& line) {
    auto elements = splitLine(line);
    if (elements.size() != 9) return std::make_pair("", nullptr);

    uint8_t type = std::stoi(elements[0]);
    UserId userId = std::stoi(elements[1]);
    auto ticker = elements[2];
    auto orderId = elements[3];
    long long timestamp = std::stol(elements[4]);

    // limit
    if (type == 1) {
        double price = std::stod(elements[6]);
        Volume volume = std::stoi(elements[7]);
        Side side = std::stoi(elements[5]) == 0 ? BUY : SELL;
        return make_pair(ticker,std::make_shared<Order>(orderId, userId, timestamp, side, LIMIT, price, volume));
    }
    // market
    if (type == 2) {
        Volume volume = std::stoi(elements[7]);
        Side side = std::stoi(elements[5]) == 0 ? BUY : SELL;
        return make_pair(ticker,std::make_shared<Order>(orderId, userId, timestamp, side, MARKET, volume));
    }
    // cancel
    if (type == 3) {
        auto cancelId = elements[8];
        return std::make_pair(ticker,std::make_shared<Order>(orderId, userId, timestamp, cancelId));
    }
    return std::make_pair("", nullptr);
}

#endif //UTILS_H
