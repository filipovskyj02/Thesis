#include "core/EngineConfig.h"
#include "core/MatchingEngine.h"
#include "iostream"

static std::string getenv_or(const char* key, const char* def) {
    if (auto p = std::getenv(key)) return std::string(p);
    return def;
}

int main()
{
    const EngineConfig cfg{
            {"AAPL", "GOOG"},       // tickers
        getenv_or("KAFKA_BROKER", "localhost:9092"),
            "orders",                   // kafkaTopic for incoming orders
            "notifications",            // kafkaTopic for outgoing notifications
            "./logs",                   // logDirectory
            "239.255.0.1",              // multicastAddress
            30001                         // multicastPort
        };
    std::cout << cfg.kafkaBroker << std::endl;
    MatchingEngine engine(cfg);
    engine.start();
}
