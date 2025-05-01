
#ifndef MATCHINGENGINE_H
#define MATCHINGENGINE_H
#include "EngineConfig.h"
#include "../common/SafeQueue.h"
#include "../common/Order.h"
#include "../io/dissemination/DisseminationEvent.h"
#include "../book/OrderBook.h"
#include "../io/dissemination/Distributor.h"

class MatchingEngine {
public:
    explicit MatchingEngine(EngineConfig  config);
    ~MatchingEngine();

    void start();
    void stop();

    MatchingEngine(const MatchingEngine&) = delete;
    MatchingEngine& operator=(const MatchingEngine&) = delete;


private:
    EngineConfig config;
    std::vector<std::unique_ptr<OrderBook>> orderBooks;

    // queues to communicate between kafka consumer and order book threads
    std::vector<std::unique_ptr<SafeQueue<std::shared_ptr<Order>>>> orderQueues;
    //SafeQueue<Event> persistQueue;
    SafeQueue<DisseminationEvent> distQueue;

    std::thread kafkaThread;
    std::vector<std::thread> bookThreads;
    //std::thread loggerThread;

    boost::asio::io_context ioContext;
    Distributor distributor;

    void runKafkaConsumer();
    void runOrderBook(size_t idx);
    //void runPersistenceLogger();
    void runMarketDistributor();
};
#endif //MATCHINGENGINE_H
