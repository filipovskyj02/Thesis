#ifndef KAFKAPRODUCER_H
#define KAFKAPRODUCER_H

#include "kafka/KafkaProducer.h"

static std::string getenv_or(const char* key, const char* def) {
    if (auto p = std::getenv(key)) return std::string(p);
    return def;
}

class OrderSender {
public:
    explicit OrderSender(std::string topic);
    ~OrderSender();
    void enqueueAsync(std::string order);
private:
    const std::string topic;
    kafka::clients::producer::KafkaProducer producer;

    std::function<void(const kafka::clients::producer::RecordMetadata&,
                     const kafka::Error&)> defaultCb;

    static kafka::clients::producer::KafkaProducer makeProducer() {
        kafka::Properties p;
        p.put("bootstrap.servers",  getenv_or("KAFKA_BROKER", "localhost:9092"));
        p.put("enable.idempotence", "true");
        p.put("linger.ms",          "2");
        return kafka::clients::producer::KafkaProducer(p);
    }
};



#endif //KAFKAPRODUCER_H
