#include <iostream>
#include <string>
#include <csignal>
#include <chrono>
#include <librdkafka/rdkafkacpp.h>
#include <bits/stdc++.h>


static volatile sig_atomic_t run = 1;

void sigterm(int sig) {
    run = 0;
}

class ExampleRebalanceCb : public RdKafka::RebalanceCb {
public:
    void rebalance_cb(RdKafka::KafkaConsumer *consumer,
                      RdKafka::ErrorCode err,
                      std::vector<RdKafka::TopicPartition *> &partitions) override {
        if (err == RdKafka::ERR__ASSIGN_PARTITIONS) {
            consumer->assign(partitions);
        } else {
            consumer->unassign();
        }
    }
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    std::string brokers = "localhost:9092";
    std::string group_id = "cpp_consumer_group";
    std::string topic = "test";

    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    std::string errstr;

    conf->set("bootstrap.servers", brokers, errstr);
    conf->set("group.id", group_id, errstr);
    conf->set("auto.offset.reset", "earliest", errstr);
    conf->set("enable.partition.eof", "true", errstr);

    ExampleRebalanceCb ex_rebalance_cb;
    conf->set("rebalance_cb", &ex_rebalance_cb, errstr);

    RdKafka::KafkaConsumer *consumer = RdKafka::KafkaConsumer::create(conf, errstr);
    if (!consumer) {
        std::cerr << "Failed to create consumer: " << errstr << std::endl;
        exit(1);
    }

    std::vector<std::string> topics = {topic};
    RdKafka::ErrorCode err = consumer->subscribe(topics);
    if (err) {
        std::cerr << "Failed to subscribe to topics: " << RdKafka::err2str(err) << std::endl;
        exit(1);
    }

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    std::cout << "Consuming messages from topic " << topic << std::endl;
    auto start = std::chrono::steady_clock::now();
    size_t recievedCnt = 0;
    std::vector<std::string> resVec(1000000);
    while (run && recievedCnt < 1000000) {
        rd_kafka_consumer_poll(consumer, 400);
        RdKafka::Message *msg = consumer->consume(1000);
        if (msg->err()) {
            if (msg->err() == RdKafka::ERR__TIMED_OUT) {
            } 
            else if (msg->err() != RdKafka::ERR__PARTITION_EOF) {
                std::cerr << "Consume error: " << msg->errstr() << std::endl;
            }
        }        
         else {
            recievedCnt++;
            if (recievedCnt == 1) {
                start = std::chrono::steady_clock::now();
            }
            //resVec[recievedCnt - 1] = static_cast<char *>(msg->payload());
            std::cout << "Received message: " << static_cast<const char *>(msg->payload()) << '\n';
        }
        delete msg;
    }
    auto end = std::chrono::steady_clock::now();
    auto duration = end - start;


    auto sec  = std::chrono::duration_cast<std::chrono::seconds>(duration);
    auto ms   = std::chrono::duration_cast<std::chrono::milliseconds>(duration - sec);
    auto us   = std::chrono::duration_cast<std::chrono::microseconds>(duration - sec - ms);

    std::cout << "Took: " 
              << sec.count() << " s, "
              << ms.count() << " ms, "
              << us.count() << " us\n";


    consumer->close();
    delete consumer;
    delete conf;

    return 0;
}