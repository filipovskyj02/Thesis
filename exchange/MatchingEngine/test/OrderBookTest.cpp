#include <gtest/gtest.h>

#include "../src/OrderBook.cpp"

// Highest bids on the top is expected
TEST(OrderBookTest, MultipleBids) {
    auto orderBook = std::make_shared<OrderBook>();

    auto firstOrder = std::make_shared<Order>(BUY, LIMIT, 123, 10);
    auto secondOrder = std::make_shared<Order>(BUY, LIMIT, 124, 10);
    auto thirdOrder = std::make_shared<Order>(BUY, LIMIT, 125, 10);
    auto fourthOrder = std::make_shared<Order>(BUY, LIMIT, 126, 10);

    orderBook->placeOrder(firstOrder);
    orderBook->placeOrder(secondOrder);
    orderBook->placeOrder(thirdOrder);
    orderBook->placeOrder(fourthOrder);

    EXPECT_EQ(orderBook->bids.size(), 4);
    EXPECT_EQ(orderBook->bids.top(), fourthOrder);
    orderBook->bids.pop();
    EXPECT_EQ(orderBook->bids.top(), thirdOrder);
    orderBook->bids.pop();
    EXPECT_EQ(orderBook->bids.top(), secondOrder);
    orderBook->bids.pop();
    EXPECT_EQ(orderBook->bids.top(), firstOrder);
}

// Lowest ask at the top is expected
TEST(OrderBookTest, MultipleAsks) {
    auto orderBook = std::make_shared<OrderBook>();

    auto firstOrder = std::make_shared<Order>(SELL, LIMIT, 123, 10);
    auto secondOrder = std::make_shared<Order>(SELL, LIMIT, 124, 10);
    auto thirdOrder = std::make_shared<Order>(SELL, LIMIT, 125, 10);
    auto fourthOrder = std::make_shared<Order>(SELL, LIMIT, 126, 10);

    orderBook->placeOrder(firstOrder);
    orderBook->placeOrder(secondOrder);
    orderBook->placeOrder(thirdOrder);
    orderBook->placeOrder(fourthOrder);

    EXPECT_EQ(orderBook->asks.size(), 4);
    EXPECT_EQ(orderBook->asks.top(), firstOrder);
    orderBook->asks.pop();
    EXPECT_EQ(orderBook->asks.top(), secondOrder);
    orderBook->asks.pop();
    EXPECT_EQ(orderBook->asks.top(), thirdOrder);
    orderBook->asks.pop();
    EXPECT_EQ(orderBook->asks.top(), fourthOrder);
}

TEST(OrderBookTest, FulfilledLimit) {
    auto orderBook = std::make_shared<OrderBook>();

    auto buy = std::make_shared<Order>(BUY, LIMIT, 100, 10);
    auto sell = std::make_shared<Order>(SELL, LIMIT, 100, 10);

    orderBook->placeOrder(buy);
    orderBook->placeOrder(sell);

    EXPECT_EQ(buy->getFilledVolume(), 10);
    EXPECT_EQ(sell->getFilledVolume(), 10);

    EXPECT_EQ(orderBook->bids.size(), 0);
    EXPECT_EQ(orderBook->asks.size(), 0);

}

TEST(OrderBookTest, PartiallyFulfilledLimit) {
    auto orderBook = std::make_shared<OrderBook>();

    auto buy = std::make_shared<Order>(BUY, LIMIT, 100, 200);
    auto sell = std::make_shared<Order>(SELL, LIMIT, 100, 10);

    orderBook->placeOrder(buy);
    orderBook->placeOrder(sell);

    EXPECT_EQ(buy->getFilledVolume(), 10);
    EXPECT_EQ(sell->getFilledVolume(), 10);

    EXPECT_EQ(buy->getRemainingVolume(), 190);
    EXPECT_EQ(sell->getRemainingVolume(), 0);

    EXPECT_EQ(orderBook->bids.size(), 1);
    EXPECT_EQ(orderBook->asks.size(), 0);
}

// Assert later order with the same price is behind the first in the bid heap
TEST(OrderBookTest, PriceTimePriorityBids) {
    auto orderBook = std::make_shared<OrderBook>();

    auto firstOrder = std::make_shared<Order>(BUY, LIMIT, 123, 10);
    // Make sure timestamp is different
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    auto secondOrder = std::make_shared<Order>(BUY, LIMIT, 123, 10);

    orderBook->placeOrder(firstOrder);
    orderBook->placeOrder(secondOrder);

    EXPECT_EQ(orderBook->bids.size(), 2);
    EXPECT_EQ(orderBook->bids.top(), firstOrder);
}

// Assert later order with the same price is behind the first in the ask heap
TEST(OrderBookTest, PriceTimePriorityAsks) {
    auto orderBook = std::make_shared<OrderBook>();

    auto firstOrder = std::make_shared<Order>(SELL, LIMIT, 123, 10);
    // Make sure timestamp is different
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    auto secondOrder = std::make_shared<Order>(SELL, LIMIT, 123, 10);

    orderBook->placeOrder(firstOrder);
    orderBook->placeOrder(secondOrder);

    EXPECT_EQ(orderBook->asks.size(), 2);
    EXPECT_EQ(orderBook->asks.top(), firstOrder);
}

TEST(OrderBookTest, MarketAgainstMultipleLimits) {
    auto orderBook = std::make_shared<OrderBook>();

    auto firstLimit = std::make_shared<Order>(SELL, LIMIT, 123, 10);
    auto secondLimit = std::make_shared<Order>(SELL, LIMIT, 124, 10);
    auto thirdLimit = std::make_shared<Order>(SELL, LIMIT, 125, 10);

    auto marketOrder = std::make_shared<Order>(BUY, MARKET, 15);

    orderBook->placeOrder(firstLimit);
    orderBook->placeOrder(secondLimit);
    orderBook->placeOrder(thirdLimit);
    orderBook->placeOrder(marketOrder);

    EXPECT_EQ(marketOrder->getFilledVolume(), 15);
    EXPECT_EQ(marketOrder->getRemainingVolume(), 0);

    EXPECT_EQ(firstLimit->getFilledVolume(), 10);
    EXPECT_EQ(firstLimit->getRemainingVolume(), 0);

    EXPECT_EQ(secondLimit->getFilledVolume(), 5);
    EXPECT_EQ(secondLimit->getRemainingVolume(), 5);

    EXPECT_EQ(thirdLimit->getFilledVolume(), 0);
    EXPECT_EQ(thirdLimit->getRemainingVolume(), 10);

    EXPECT_EQ(orderBook->asks.size(), 2);
}

TEST(OrderBookTest, MarketOrderNoLimits) {
    auto orderBook = std::make_shared<OrderBook>();
    auto marketOrder = std::make_shared<Order>(BUY, MARKET, 123, 10);

    EXPECT_FALSE(orderBook->placeOrder(marketOrder));
}

TEST(OrderBookTest, MarketOrderNotEnoughLiquidity) {
    auto orderBook = std::make_shared<OrderBook>();
    auto marketOrder = std::make_shared<Order>(BUY, MARKET, 30);

    auto firstLimit = std::make_shared<Order>(SELL, LIMIT, 123, 10);
    auto secondLimit = std::make_shared<Order>(SELL, LIMIT, 124, 10);

    orderBook->placeOrder(firstLimit);
    orderBook->placeOrder(secondLimit);

    EXPECT_FALSE(orderBook->placeOrder(marketOrder));
    EXPECT_EQ(firstLimit->getFilledVolume(), 10);
    EXPECT_EQ(secondLimit->getFilledVolume(), 10);

    EXPECT_EQ(marketOrder->getFilledVolume(), 20);
    EXPECT_EQ(marketOrder->getRemainingVolume(), 10);

    EXPECT_EQ(orderBook->asks.size(), 0);
    EXPECT_EQ(orderBook->bids.size(), 0);
}

TEST(OrderBookTest, CanceledOrderIsNotFulfilledByLimit) {
    auto orderBook = std::make_shared<OrderBook>();
    auto limitOrderSell = std::make_shared<Order>(SELL, LIMIT, 100, 10);
    auto limitOrderBuy = std::make_shared<Order>(BUY, LIMIT, 100, 10);

    orderBook->placeOrder(limitOrderSell);
    orderBook->cancelOrderLazy(limitOrderSell);

    orderBook->placeOrder(limitOrderBuy);


    EXPECT_EQ(limitOrderBuy->getFilledVolume(), 0);
    EXPECT_EQ(limitOrderSell->getFilledVolume(), 0);

    EXPECT_TRUE(limitOrderSell->isCanceled());

    EXPECT_EQ(orderBook->bids.size(), 1);
    EXPECT_EQ(orderBook->asks.size(), 0);
}


TEST(OrderBookTest, CanceledOrderIsNotFulfilledByMarket) {
    auto orderBook = std::make_shared<OrderBook>();
    auto limitOrderSell = std::make_shared<Order>(SELL, LIMIT, 100, 10);
    auto marketOrderBuy = std::make_shared<Order>(BUY, MARKET, 10);

    orderBook->placeOrder(limitOrderSell);
    orderBook->cancelOrderLazy(limitOrderSell);

    EXPECT_FALSE(orderBook->placeOrder(marketOrderBuy));

    EXPECT_EQ(marketOrderBuy->getFilledVolume(), 0);
    EXPECT_EQ(limitOrderSell->getFilledVolume(), 0);

    EXPECT_TRUE(limitOrderSell->isCanceled());

    EXPECT_EQ(orderBook->bids.size(), 0);
    EXPECT_EQ(orderBook->asks.size(), 0);
}

TEST(OrderBookTest, MarketOrderIsNotCancellable) {
    auto orderBook = std::make_shared<OrderBook>();
    auto marketOrderBuy = std::make_shared<Order>(BUY, MARKET, 10);

    EXPECT_FALSE(orderBook->cancelOrderLazy(marketOrderBuy));
}

TEST(OrderBookTest, FulfilledOrderIsNotCancellable) {
    auto orderBook = std::make_shared<OrderBook>();
    auto limitSell = std::make_shared<Order>(SELL, LIMIT, 100, 40);
    auto limitOrderBuy = std::make_shared<Order>(BUY, LIMIT, 100, 10);

    orderBook->placeOrder(limitSell);
    orderBook->placeOrder(limitOrderBuy);

    EXPECT_EQ(limitOrderBuy->getRemainingVolume(), 0);
    EXPECT_FALSE(orderBook->cancelOrderLazy(limitOrderBuy));
}

TEST(OrderBookTest, PartiallyFulfilledOrderIsCancellable) {
    auto orderBook = std::make_shared<OrderBook>();
    auto limitSell = std::make_shared<Order>(SELL, LIMIT, 100, 40);
    auto limitOrderBuy = std::make_shared<Order>(BUY, LIMIT, 100, 10);

    orderBook->placeOrder(limitSell);
    orderBook->placeOrder(limitOrderBuy);

    EXPECT_EQ(limitOrderBuy->getRemainingVolume(), 0);
    EXPECT_EQ(limitSell->getRemainingVolume(), 30);
    EXPECT_TRUE(orderBook->cancelOrderLazy(limitSell));
}