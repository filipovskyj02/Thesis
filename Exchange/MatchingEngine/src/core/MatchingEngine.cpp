#include "MatchingEngine.h"

#include <utility>
#include <fstream>

MatchingEngine::MatchingEngine(EngineConfig cfg)
  : config(std::move(cfg)),
    distributor(ioContext,
                config.multicastAddress,
                config.multicastPort,
                distQueue),
    logger(config.logDirectory,
           persistQueue),
    kafkaReader(orderQueues,
        tickerToIndex,
        config.kafkaBroker,
        config.kafkaTopicInOrders),
    kafkaWriter(notificationQueue,
        config.kafkaBroker,
        config.kafkaTopicOutNotifications)
{}

MatchingEngine::~MatchingEngine() {
    stop();
}

void MatchingEngine::start() {
    runMarketDistributor();
    runPersistenceLogger();
    runUserNotifier();

    for (size_t i = 0; i < config.tickers.size(); ++i) {
        orderQueues.emplace_back(std::make_unique<SafeQueue<std::shared_ptr<Order>>>());
        tickerToIndex[config.tickers[i]] = i;
        orderBooks.emplace_back(std::make_unique<OrderBook>(config.tickers[i], *orderQueues[i], distQueue, persistQueue, notificationQueue));
        bookThreads.emplace_back(&MatchingEngine::runOrderBook, this, i);
    }

    runKafkaConsumer();
}

void MatchingEngine::stop() {
    kafkaReader.stop();
    for (auto& q : orderQueues) {
        q->close();
    }

    for (auto& t : bookThreads) {
        if (t.joinable()) {
            t.join();
        }
    }
    distributor.stop();
    logger.stop();
    kafkaWriter.stop();
}

void MatchingEngine::runKafkaConsumer() {
   kafkaReader.start();
}

void MatchingEngine::runMarketDistributor() {
    distributor.start();
}

void MatchingEngine::runOrderBook(size_t idx) {
    orderBooks[idx]->run();
}

void MatchingEngine::runPersistenceLogger() {
    logger.start();
}

void MatchingEngine::runUserNotifier() {
    kafkaWriter.start();
}

