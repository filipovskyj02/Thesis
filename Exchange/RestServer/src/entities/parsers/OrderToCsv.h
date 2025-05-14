#ifndef ORDERTOCSVPARSER_H
#define ORDERTOCSVPARSER_H
#include <string>
#include "../CancelOrderReq.h"
#include "../MarketOrderReq.h"
#include "../LimitOrderReq.h"




inline std::string toCsv(const LimitOrderReq& o) {
    return std::to_string(1) + "," +                      // type=1 limit
           std::to_string(o.userId) + "," +
           o.ticker + "," +
           o.orderId + "," +
           std::to_string(o.timestamp) + "," +
           std::to_string(o.side) + "," +
           std::to_string(o.price) + "," +
           std::to_string(o.volume) + ",\n";
}
inline std::string toCsv(const MarketOrderReq& o) {
    return std::to_string(2) + "," +                      // type=2 market
           std::to_string(o.userId) + "," +
           o.ticker + "," +
           o.orderId + "," +
           std::to_string(o.timestamp) + "," +
           std::to_string(o.side) + "," +
           "," +                                    // empty price
           std::to_string(o.volume) + ",\n";
}
inline std::string toCsv(const CancelOrderReq& o) {
    return std::to_string(3) + "," +                      // type=3 cancel
           std::to_string(o.userId) + "," +
           o.ticker + "," +
           o.orderId + "," +
           std::to_string(o.timestamp) + ",,,," +        // no side/price/volume
           o.cancelTargetId;
}
#endif //ORDERTOCSVPARSER_H
