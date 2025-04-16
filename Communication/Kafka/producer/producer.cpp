#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <librdkafka/rdkafkacpp.h>

int main() {
    std::string brokers = "localhost:9092";
    std::string topic_name = "test";
    std::string message = "1 19219219 BUY LIMIT 100 20.3";
    const int TOTAL_MESSAGES = 1000000;

    std::string errstr;

    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    conf->set("bootstrap.servers", brokers, errstr);

    RdKafka::Producer *producer = RdKafka::Producer::create(conf, errstr);
    if (!producer) {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        return 1;
    }

    RdKafka::Topic *topic = RdKafka::Topic::create(producer, topic_name, nullptr, errstr);
    if (!topic) {
        std::cerr << "Failed to create topic: " << errstr << std::endl;
        return 1;
    }

    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < TOTAL_MESSAGES; ++i) {
        std::string msg = std::to_string(i) + " " + message;
        RdKafka::ErrorCode resp = producer->produce(
            topic,
            RdKafka::Topic::PARTITION_UA,
            RdKafka::Producer::RK_MSG_COPY,
            const_cast<char *>(msg.c_str()),
            msg.size(),
            nullptr,
            nullptr
        );

        if (resp != RdKafka::ERR_NO_ERROR) {
            std::cerr << "Produce failed for msg " << i << ": " << RdKafka::err2str(resp) << std::endl;
        }

        // Allow queue to drain to avoid local queue overflow
        producer->poll(0);
    }

    producer->flush(5000);
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(end - start).count();

    std::cout << "Sent " << TOTAL_MESSAGES << " messages in "
              << std::fixed << std::setprecision(3) << duration << " seconds\n";
    std::cout << "Throughput: "
              << std::fixed << std::setprecision(0)
              << (TOTAL_MESSAGES / duration) << " messages/sec\n";

    delete topic;
    delete producer;
    delete conf;

    return 0;
}
