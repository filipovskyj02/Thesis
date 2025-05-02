#ifndef ORDERPARSER_H
#define ORDERPARSER_H

#include <chrono>
#include <optional>
#include <string>

#include <crow.h>
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

#define REQUIRE_FIELD(r,fld,TYPE)                           \
if (! (r.has(#fld)                                          \
&& r[#fld].t() == crow::json::type::TYPE) )                 \
return std::nullopt;

inline std::optional<LimitOrderReq> parseLimit(const crow::request& req) {
    auto r = crow::json::load(req.body);
    if (!r) return std::nullopt;

    REQUIRE_FIELD(r, userId, Number);
    REQUIRE_FIELD(r, ticker, String);
    REQUIRE_FIELD(r, side,   Number);
    REQUIRE_FIELD(r, price,  Number);
    REQUIRE_FIELD(r, volume, Number);

    return LimitOrderReq{
        static_cast<int32_t>(r["userId"].i()),
        r["ticker"].s(),
        makeUuid(),
        nowEpochMs(),
        static_cast<int   >(r["side"  ].i()),
        /*both .d() & .i() are valid for Number*/ r["price" ].d(),
        static_cast<int64_t>(r["volume"].i())
      };
}

inline std::optional<MarketOrderReq> parseMarket(const crow::request& req) {
    auto r = crow::json::load(req.body);
    if (!r) return std::nullopt;

    REQUIRE_FIELD(r, userId, Number);
    REQUIRE_FIELD(r, ticker, String);
    REQUIRE_FIELD(r, side,   Number);
    REQUIRE_FIELD(r, volume, Number);

    return MarketOrderReq{
        static_cast<int32_t>(r["userId"].i()),
        r["ticker"].s(),
        makeUuid(),
        nowEpochMs(),
        static_cast<int   >(r["side"  ].i()),
        static_cast<int64_t>(r["volume"].i())
      };
}

inline std::optional<CancelOrderReq> parseCancel(const crow::request& req) {
    auto r = crow::json::load(req.body);
    if (!r) return std::nullopt;

    REQUIRE_FIELD(r, userId,        Number);
    REQUIRE_FIELD(r, ticker,        String);
    REQUIRE_FIELD(r, cancelTargetId,String);

    return CancelOrderReq{
        static_cast<int32_t>(r["userId"        ].i()),
        r["ticker"        ].s(),
        makeUuid(),
        nowEpochMs(),
        r["cancelTargetId"].s()
      };
}

#undef REQUIRE_FIELD

#endif // ORDERPARSER_H