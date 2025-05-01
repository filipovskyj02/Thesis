#include <iostream>
#include "kafka/KafkaProducer.h"

#include <cstdlib>
#include <iostream>
#include <string>


int main() {
    using namespace kafka;
    using namespace kafka::clients::producer;

    const std::string brokers = "localhost:9092";
    const Properties props({{"bootstrap.servers", brokers}});
    // Create a producer
    KafkaProducer producer(props);

    // Prepare a message
    std::string line= "1,10,AAPL,ord1,1618886400,1,123.45,1,\n";

    ProducerRecord record("orders", NullKey, Value(line.c_str(), line.size()));

    // Prepare delivery callback
    auto deliveryCb = [](const RecordMetadata& metadata, const Error& error) {
        if (!error) {
            std::cout << "Message delivered: " << metadata.toString() << std::endl;
        } else {
            std::cerr << "Message failed to be delivered: " << error.message() << std::endl;
        }
    };

    for (int i = 1; i <= 1900000; i++) {
        producer.send(record, deliveryCb);

    }

    // Close the producer explicitly(or not, since RAII will take care of it)
    producer.close();
}