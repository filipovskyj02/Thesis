#include "core/EngineConfig.h"
#include "core/MatchingEngine.h"


int main()
{
    const EngineConfig cfg{
            {"AAPL", "GOOG"},       // tickers
            "localhost:9092",           // kafkaBroker
            "orders",                   // kafkaTopic
            "./logs",                   // logDirectory
            "239.255.0.1",              // multicastAddress
            30001                         // multicastPort
        };

    MatchingEngine engine(cfg);
    engine.start();
}
