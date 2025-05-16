#include "OrderSender.h"



using namespace kafka;
using namespace kafka::clients::producer;

::OrderSender::OrderSender(const std::string topic)
  : topic(topic),
    producer(makeProducer()),
    defaultCb(
      [](auto& md, auto& err) {
          if (err) {
            std::cerr << "[OrderSender] delivery failed: "
                      << err.message() << "\n";
          }
          else {
         //     std::cout << "Message delivered: " << md.toString() << std::endl;
          }
      })
{}

void ::OrderSender::enqueueAsync(const std::string order) {
    ProducerRecord record(topic, NullKey, Value(order.c_str(), order.size()));
    producer.send(record, defaultCb, KafkaProducer::SendOption::ToCopyRecordValue);
}

OrderSender::~OrderSender() {
    producer.close();
}

