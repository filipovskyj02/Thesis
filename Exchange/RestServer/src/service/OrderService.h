
#ifndef ORDERSERVICE_H
#define ORDERSERVICE_H
#include <crow/http_response.h>
#include "../entities/parsers/RequestToOrder.h"
#include "../entities/parsers/OrderToCsv.h"

#include <crow.h>
#include "../entities/LimitOrderReq.h"
#include "../entities/MarketOrderReq.h"
#include "../entities/CancelOrderReq.h"

class OrderService {
public:
    explicit OrderService(OrderSender& sender) : sender(sender) {}

    crow::response processLimit(const crow::request& req) {
        auto o = parseLimit(req);
        if (!o) return {400, "Bad or missing fields in LimitOrderReq\n"};
        const auto& ord = *o;
        sender.enqueueAsync(toCsv(ord));
        return crow::response{200, ord.orderId};
    }

    crow::response processMarket(const crow::request& req) {
        auto o = parseMarket(req);
        if (!o) return {400, "Bad or missing fields in MarketOrderReq\n"};
        const auto& ord = *o;
        sender.enqueueAsync(toCsv(ord));
        return crow::response{200, ord.orderId};
    }

    crow::response processCancel(const crow::request& req) {
        auto o = parseCancel(req);
        if (!o) return {400, "Bad or missing fields in CancelOrderReq\n"};
        const auto& ord = *o;
        sender.enqueueAsync(toCsv(ord));
        return crow::response{200, ord.orderId};
    }

private:
    OrderSender& sender;
};


#endif //ORDERSERVICE_H
