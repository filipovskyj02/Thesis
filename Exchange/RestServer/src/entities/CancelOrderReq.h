
#ifndef CANCELORDERREQ_H
#define CANCELORDERREQ_H
#include <string>

struct CancelOrderReq {
    int32_t     userId;
    std::string ticker;
    std::string orderId;      // server generated UUID
    long long    timestamp;
    std::string cancelTargetId;
};
#endif //CANCELORDERREQ_H
