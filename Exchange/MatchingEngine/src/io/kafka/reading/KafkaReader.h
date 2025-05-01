
#ifndef KAFKAREADER_H
#define KAFKAREADER_H
#include <memory>
#include <vector>
#include "../../../common/SafeQueue.h"
#include "../../../common/Order.h"
#include <kafka/KafkaConsumer.h>

class KafkaReader {
public:
    explicit KafkaReader(std::vector<std::unique_ptr<SafeQueue<std::shared_ptr<Order>>>> &orderQueues,
                         std::unordered_map<std::string, size_t>& tickerToIndex,
                         std::string kafkaBroker,
                         std::string kafkaTopic);


    ~KafkaReader();

    void start();

    void stop();

private:
    void run();
    std::vector<std::unique_ptr<SafeQueue<std::shared_ptr<Order>>>> &orderQueues;
    std::unordered_map<std::string, size_t>& tickerToIndex;
    std::string kafkaBroker;
    std::string kafkaTopic;
    std::atomic<bool> running;
};
#endif //KAFKAREADER_H
