
#ifndef MATCHINGENGINE_H
#define MATCHINGENGINE_H
#include "EngineConfig.h"
#include "../common/SafeQueue.h"
#include "../common/Order.h"
#include "../io/dissemination/DisseminationEvent.h"
#include "../book/OrderBook.h"
#include "../io/dissemination/Distributor.h"
#include "../io/kafka/reading/KafkaReader.h"
#include "../io/logging/Logger.h"
#include "../io/logging/LogEvent.h"


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
    std::unordered_map<std::string, size_t> tickerToIndex;

    SafeQueue<LogEvent> persistQueue;
    SafeQueue<DisseminationEvent> distQueue;

    std::vector<std::thread> bookThreads;

    boost::asio::io_context ioContext;
    Distributor distributor;
    Logger logger;
    KafkaReader kafkaReader;

    void runKafkaConsumer();
    void runOrderBook(size_t idx);
    void runPersistenceLogger();
    void runMarketDistributor();
};
#endif //MATCHINGENGINE_H
