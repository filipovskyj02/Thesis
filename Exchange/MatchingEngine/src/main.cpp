#include "core/EngineConfig.h"
#include "core/MatchingEngine.h"


int main()
{
    const EngineConfig cfg{
            {"AAPL", "GOOG"},           // tickers
            "localhost:9092",           // kafkaBroker
            "orders",                   // kafkaTopic
            "./logs",                   // logDirectory (unused for now)
            "239.255.0.1",              // multicastAddress
            30001                       // multicastPort
        };

    // 2) Construct and start
    MatchingEngine engine(cfg);
    engine.start();
    std::this_thread::sleep_for(std::chrono::seconds(1000));
    engine.stop();
}
