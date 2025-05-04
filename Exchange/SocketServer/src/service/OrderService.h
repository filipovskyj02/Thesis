
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

    std::string processCsvLine(const std::string& line) {
        auto f = splitLine(line);
        if (f.empty()) return "ERR :bad_format\n";

        try {
            int orderType = std::stoi(f[0]);
            switch (orderType) {
                case 1: return handleLimit(f);
                case 2: return handleMarket(f);
                case 3: return handleCancel(f);
                default: return "ERR : unknown order type\n";
            }
        } catch(...) {
            return "ERR : failed to parse order\n";
        }
    }

private:
    OrderSender& sender_;

    std::string handleLimit(const std::vector<std::string>& f) {
        auto o = parseLimitCsv(f, 1);
        if (!o) return "ERR : failed to parse limit order\n";
        auto& ord = *o;
        sender_.enqueueAsync(toCsv(ord));
        return "ACK " + ord.orderId + "\n";
    }

    std::string handleMarket(const std::vector<std::string>& f) {
        auto o = parseMarketCsv(f,1);
        if (!o) return "ERR : failed to parse market order\n";
        auto& ord = *o;
        sender_.enqueueAsync(toCsv(ord));
        return "ACK " + ord.orderId + "\n";
    }

    std::string handleCancel(const std::vector<std::string>& f) {
        auto o = parseCancelCsv(f,1);
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
