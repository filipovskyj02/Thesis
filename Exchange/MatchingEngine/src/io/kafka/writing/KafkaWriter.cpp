#include "KafkaWriter.h"
#include <kafka/KafkaProducer.h>

using namespace kafka;
using namespace kafka::clients::producer;

KafkaWriter::KafkaWriter(SafeQueue<NotificationEvent>& eventQueue,
                         std::string kafkaBroker,
                         std::string kafkaTopic)
  : eventQueue(eventQueue)
  , kafkaBroker(kafkaBroker)
  , kafkaTopic(kafkaTopic)
{}

KafkaWriter::~KafkaWriter() {
    stop();
}

void KafkaWriter::start() {
    if (running.exchange(true)) return;
    writerThread = std::thread(&KafkaWriter::run, this);
}

void KafkaWriter::stop() {
    if (!running.exchange(false)) {
        return;
    }
    eventQueue.close();
    if (writerThread.joinable()) {
        writerThread.join();
    }
}

void KafkaWriter::run() {
    Properties props;
    props.put("bootstrap.servers", kafkaBroker);
    props.put("enable.idempotence", "true");

    KafkaProducer producer(props);

    auto cb = [](const RecordMetadata& meta, const Error& err) {
        if (err) {
            std::cerr << "KafkaWriter delivery failed: " << err.message() << std::endl;
        }
    };

    while (running) {
        auto opt = eventQueue.pop();
        if (!opt) break;

        std::string payload = std::visit([](auto&& ev){
            return toString(ev);
        }, *opt);

        ProducerRecord record(kafkaTopic,
                              NullKey,
                              Value(payload.c_str(), payload.size()));
        producer.send(record, cb, KafkaProducer::SendOption::ToCopyRecordValue);
    }

    producer.close();
}