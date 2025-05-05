#include "KafkaReader.h"
#include <kafka/KafkaConsumer.h>
#include "../src/common/Utils.h"

using namespace kafka;
using namespace kafka::clients::consumer;

KafkaReader::KafkaReader(std::vector<std::unique_ptr<SafeQueue<std::shared_ptr<Order>>>> &orderQueues,
                         std::unordered_map<std::string, size_t>& tickerToIndex,
                         std::string kafkaBroker,
                         std::string kafkaTopic)
            : orderQueues(orderQueues),
                tickerToIndex(tickerToIndex),
                kafkaBroker(kafkaBroker),
                kafkaTopic(kafkaTopic)
{}
KafkaReader::~KafkaReader() {
    stop();
}
void KafkaReader::start() {
    if (running.exchange(true)) {
        return;
    }
    run();
}
void KafkaReader::stop() {
    running = false;
}

void KafkaReader::run() {
    Properties props({
  {"bootstrap.servers", { kafkaBroker }},
  {"group.id",           {"matching-engine-group"}},
});

    KafkaConsumer consumer(props);
    consumer.subscribe({ kafkaTopic }, NullRebalanceCallback, std::chrono::milliseconds(300000));

    while (running) {
        auto records = consumer.poll(std::chrono::milliseconds(100));

        for (const auto& record: records) {
            if (record.error()) {
                // log or smth
                continue;
            }
            //std::cout << "recieved message in core matching engine: " << record.value().toString() << std::endl;
            auto res = parseRecord(record.value().toString());
            if (res.first.empty() or !res.second)    {
                std::cout << "Failed to parse order" << std::endl;
                continue;
            }
            if (tickerToIndex.contains(res.first)) {
                orderQueues[tickerToIndex.at(res.first)]->push(res.second);
            }
            else {
                std::cout << "Invalid ticker" << std::endl;
            }
        }
    }
}



