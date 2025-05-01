#include <gtest/gtest.h>
#include "../src/common/Utils.h"

// msgType,userId,ticker,orderId,timestamp,side,price,volume, cancelId

TEST(KafkaReaderParse, RejectsWrongFieldCount) {
    // Only 8 fields instead of 9
    EXPECT_EQ(parseRecord("1,10,AAPL,ord1,1618886400,0,123.45,100").second, nullptr);
}

TEST(KafkaReaderParse, ParsesLimitOrder) {
    // type=1 (limit), side=0 (BUY)
    auto pr = parseRecord("1,10,AAPL,ord1,1618886400,0,123.45,100,\n");
    auto o = pr.second;
    ASSERT_NE(o, nullptr);
    EXPECT_EQ(o->getOrderType(), OrderType::LIMIT);
    EXPECT_EQ(o->getSide(),      Side::BUY);
    EXPECT_DOUBLE_EQ(o->getPrice(),  123.45);
    EXPECT_EQ(o->getOriginalVolume(), (Volume)100);
    EXPECT_EQ(o->getId(),       "ord1");
    EXPECT_EQ(o->getUserId(),   10);
    EXPECT_EQ(o->getTimestamp(), 1618886400LL);
    EXPECT_EQ(pr.first, "AAPL");
}

TEST(KafkaReaderParse, ParsesMarketOrder) {
    // type=2 (market), side=1 (SELL)
    auto pr = parseRecord("2,99,GOOG,0dc66a92-110a-4d44-ba1f-486e39c27fb2,1619000000,1,,250,\n");
    auto o = pr.second;
    ASSERT_NE(o, nullptr);
    EXPECT_EQ(o->getId(),       "0dc66a92-110a-4d44-ba1f-486e39c27fb2");
    EXPECT_EQ(o->getOrderType(), OrderType::MARKET);
    EXPECT_EQ(o->getSide(),      Side::SELL);
    EXPECT_EQ(o->getOriginalVolume(), (Volume) 250);
    EXPECT_EQ(o->getPrice(),     0.0);      // price field ignored for MARKET
    EXPECT_EQ(pr.first, "GOOG");

}

TEST(KafkaReaderParse, ParsesCancelOrder) {
    // type=3 (cancel), cancelId in field 8
    auto pr = parseRecord("3,123,MSFT,ordC,1619005000,,,,ord1");
    auto o = pr.second;
    ASSERT_NE(o, nullptr);
    EXPECT_EQ(o->getOrderType(), OrderType::CANCEL);
    EXPECT_EQ(o->getCancelTarget(), "ord1");
    EXPECT_EQ(pr.first, "MSFT");
}

TEST(KafkaReaderParse, RejectsUnknownType) {
    // type=9 is unsupported
    EXPECT_EQ(parseRecord("9,1,AAPL,ord,1619000000,0,100.0,10,").second, nullptr);
}
