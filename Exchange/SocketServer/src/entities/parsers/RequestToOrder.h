#ifndef ORDERPARSER_H
#define ORDERPARSER_H

#include <chrono>
#include <optional>
#include <string>

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "../LimitOrderReq.h"
#include "../MarketOrderReq.h"
#include "../CancelOrderReq.h"

inline boost::uuids::random_generator& uuidGen() {
    static thread_local boost::uuids::random_generator gen;
    return gen;
}

inline std::string makeUuid() {
    return to_string(uuidGen()());
}

inline long long nowEpochMs() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
             system_clock::now().time_since_epoch()
           ).count();
}

inline std::optional<LimitOrderReq>
parseLimitCsv(const std::vector<std::string>& f, int32_t userId) {
    if (f.size() != 5) return std::nullopt;
    const auto& ticker = f[1];
    int side;
    double price;
    int64_t volume;
    try {
        side   = std::stoi(f[2]);
        price  = std::stod(f[3]);
        volume = std::stoll(f[4]);
    } catch (...) {
        return std::nullopt;
    }
    return LimitOrderReq{
        userId,
        ticker,
        makeUuid(),
        nowEpochMs(),
        side,
        price,
        volume
      };
}

inline std::optional<MarketOrderReq>
parseMarketCsv(const std::vector<std::string>& f, int32_t userId) {
    if (f.size() != 4) return std::nullopt;
    const auto& ticker = f[1];
    int side;
    int64_t volume;
    try {
        side   = std::stoi(f[2]);
        volume = std::stoll(f[3]);
    } catch (...) {
        return std::nullopt;
    }
    return MarketOrderReq{
        userId,
        ticker,
        makeUuid(),
        nowEpochMs(),
        side,
        volume
      };
}

inline std::optional<CancelOrderReq>
parseCancelCsv(const std::vector<std::string>& f, int32_t userId) {
    if (f.size() != 3) return std::nullopt;
    const auto& ticker        = f[1];
    const auto& cancelTarget  = f[2];
    if (cancelTarget.empty()) return std::nullopt;

    return CancelOrderReq{
        userId,
        ticker,
        makeUuid(),
        nowEpochMs(),
        cancelTarget
      };
}

#endif // ORDERPARSER_H