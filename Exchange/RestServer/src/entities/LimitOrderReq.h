
#ifndef LIMITORDERREQ_H
#define LIMITORDERREQ_H
#include <cstdint>
#include <string>

struct LimitOrderReq {
    int32_t   userId;
    std::string ticker;
    std::string orderId; // server generated UUID
    long long   timestamp;
    int       side;         // 0=BUY, 1=SELL
    double    price;
    int64_t   volume;
};
#endif //LIMITORDERREQ_H
