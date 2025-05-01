#include "MatchingEngine.h"

#include <utility>
#include <fstream>
#include <sstream>

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
        config.kafkaTopic)
{
    orderQueues.reserve(config.tickers.size());
}

MatchingEngine::~MatchingEngine() {
    stop();
}

void MatchingEngine::start() {
    runMarketDistributor();
    runPersistenceLogger();

    for (size_t i = 0; i < config.tickers.size(); ++i) {
        orderQueues.emplace_back(
            std::make_unique<SafeQueue<std::shared_ptr<Order>>>()
        );
    }
    for (size_t i = 0; i < config.tickers.size(); ++i) {
        tickerToIndex[config.tickers[i]] = i;
        orderBooks.emplace_back(std::make_unique<OrderBook>(config.tickers[i], *orderQueues[i], distQueue, persistQueue));
        bookThreads.emplace_back(&MatchingEngine::runOrderBook, this, i);
    }

    runKafkaConsumer();

}

void MatchingEngine::stop() {
    for (auto& q : orderQueues) {
        q->close();
    }

    for (auto& t : bookThreads) {
        if (t.joinable()) {
            t.join();
        }
    }
    kafkaReader.stop();
    distQueue.close();
    distributor.stop();
    logger.stop();

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
