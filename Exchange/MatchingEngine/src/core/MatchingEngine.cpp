#include "MatchingEngine.h"

#include <utility>
#include <fstream>
#include <sstream>

MatchingEngine::MatchingEngine(EngineConfig cfg)
  : config(std::move(cfg)),
    distributor(ioContext,
                config.multicastAddress,
                config.multicastPort,
                distQueue)
{
    orderQueues.reserve(config.tickers.size());
}

MatchingEngine::~MatchingEngine() {
    stop();
}

void MatchingEngine::start() {
    runMarketDistributor();
    kafkaThread = std::thread(&MatchingEngine::runKafkaConsumer, this);

    for (size_t i = 0; i < config.tickers.size(); ++i) {
        orderQueues.emplace_back(
            std::make_unique<SafeQueue<std::shared_ptr<Order>>>()
        );
    }

    for (size_t i = 0; i < config.tickers.size(); ++i) {
        orderBooks.emplace_back(std::make_unique<OrderBook>(config.tickers[i], *orderQueues[i], distQueue));
    }
    // one thread per ticker
    for (size_t i = 0; i < config.tickers.size(); ++i) {
        bookThreads.emplace_back(&MatchingEngine::runOrderBook, this, i);
    }

}

void MatchingEngine::stop() {
    if (kafkaThread.joinable())
        kafkaThread.join();

    for (auto& q : orderQueues) {
        q->close();
    }

    for (auto& t : bookThreads) {
        if (t.joinable()) {
            t.join();
        }
    }

    distQueue.close();
    distributor.Stop();

}

void MatchingEngine::runKafkaConsumer() {
    std::string path = "../../../../Benchmarking/DataGen/outputs/0-2000-0.670000-0-04-04-2025 00-46-52.txt";  // add this to EngineConfig
    std::ifstream inputFile(path);
    if (!inputFile.is_open()) {
        throw std::runtime_error("Cannot open data file: " + path);
    }

    int initialCount, ordersCount;
    inputFile >> initialCount >> ordersCount;

    for (int i = 0; i < initialCount; ++i) {
        OrderId  id;     std::string side, type;
        Price    price;  Volume vol;
        inputFile >> id >> side >> type >> price >> vol;

        auto order = std::make_shared<Order>(
            convertSide(side),
            convertType(type),
            price,
            vol
        );
        orderQueues[rand() % config.tickers.size()]->push(order);
    }

    for (int i = 0; i < ordersCount; ++i) {
        OrderId  id;     std::string side, type;
        Price    price;  Volume vol;
        inputFile >> id >> side >> type >> price >> vol;
            std::shared_ptr<Order> order;
            if (type == "MARKET") {
                order = std::make_shared<Order>(
                    convertSide(side),
                    convertType(type),
                    vol
                );
            } else {
                order = std::make_shared<Order>(
                    convertSide(side),
                    convertType(type),
                    price,
                    vol
                );
            }
            orderQueues[rand() % config.tickers.size() ]->push(order);

    }

    inputFile.close();
    for (auto& q : orderQueues) q->close();
}

void MatchingEngine::runMarketDistributor() {
    distributor.Start();
}


void MatchingEngine::runOrderBook(size_t idx) {
    orderBooks[idx]->run();
}