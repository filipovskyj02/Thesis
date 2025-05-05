
#ifndef ORDERSERVICE_H
#define ORDERSERVICE_H
#include "../entities/parsers/RequestToOrder.h"
#include "../entities/parsers/OrderToCsv.h"

#include "../entities/LimitOrderReq.h"
#include "../entities/MarketOrderReq.h"
#include "../entities/CancelOrderReq.h"

class OrderService {
public:
    explicit OrderService(OrderSender& sender)
      : sender_(sender)
    {}

    std::string processCsvLine(const std::string& line, const int32_t userId) {
        auto fields = splitLine(line);
        if (fields.empty()) return "ERR :bad_format\n";

        try {
            int orderType = std::stoi(fields[0]);
            switch (orderType) {
                case 1: return handleLimit(fields, userId);
                case 2: return handleMarket(fields, userId);
                case 3: return handleCancel(fields, userId);
                default: return "ERR : unknown order type\n";
            }
        } catch(...) {
            return "ERR : failed to parse order\n";
        }
    }

private:
    OrderSender& sender_;

    std::string handleLimit(const std::vector<std::string>& f, const int32_t userId) {
        auto o = parseLimitCsv(f, userId);
        if (!o) return "ERR : failed to parse limit order\n";
        auto& ord = *o;
        sender_.enqueueAsync(toCsv(ord));
        return "ACK " + ord.orderId + "\n";
    }

    std::string handleMarket(const std::vector<std::string>& f, const int32_t userId) {
        auto o = parseMarketCsv(f,userId);
        if (!o) return "ERR : failed to parse market order\n";
        auto& ord = *o;
        sender_.enqueueAsync(toCsv(ord));
        return "ACK " + ord.orderId + "\n";
    }

    std::string handleCancel(const std::vector<std::string>& f, const int32_t userId) {
        auto o = parseCancelCsv(f,userId);
        if (!o) return "ERR : failed to parse cancel order\n";
        auto& ord = *o;
        sender_.enqueueAsync(toCsv(ord));
        return "ACK " + ord.orderId + "\n";
    }

    inline std::vector<std::string> splitLine(const std::string& s) {
        std::vector<std::string> out;
        std::string part;
        std::istringstream ss(s);
        while (std::getline(ss, part, ',')) out.push_back(part);
        return out;
    }
};

#endif //ORDERSERVICE_H
