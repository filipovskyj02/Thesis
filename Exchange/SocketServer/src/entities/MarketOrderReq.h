
#ifndef MARKETORDERREQ_H
#define MARKETORDERREQ_H
#include <cstdint>
#include <string>

struct MarketOrderReq {
    int32_t     userId;
    std::string ticker;
    std::string orderId; // server generated UUID
    long long   timestamp;
    int         side;       // 0=BUY, 1=SELL
    int64_t     volume;
};
#endif //MARKETORDERREQ_H
