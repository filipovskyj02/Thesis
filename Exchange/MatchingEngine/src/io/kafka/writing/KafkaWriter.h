
#ifndef KAFKAWRITER_H
#define KAFKAWRITER_H
#include <string>

#include "NotificationEvent.h"
#include "../../../common/SafeQueue.h"


class KafkaWriter {
public:
    explicit KafkaWriter(SafeQueue<NotificationEvent>& eventQueue,
                         std::string kafkaBroker,
                         std::string kafkaTopic);


    ~KafkaWriter();

    void start();

    void stop();

private:
    void run();
    SafeQueue<NotificationEvent>& eventQueue;
    std::thread writerThread;
    std::string kafkaBroker;
    std::string kafkaTopic;
    std::atomic<bool> running;
};



#endif //KAFKAWRITER_H
