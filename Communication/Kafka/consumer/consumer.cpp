#include <iostream>
#include <string>
#include <csignal>
#include <librdkafka/rdkafkacpp.h>

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

    while (run) {
        RdKafka::Message *msg = consumer->consume(1000);
        if (msg->err()) {
            if (msg->err() == RdKafka::ERR__TIMED_OUT) {
                // Do nothing — expected idle poll
            } else if (msg->err() != RdKafka::ERR__PARTITION_EOF) {
                std::cerr << "Consume error: " << msg->errstr() << std::endl;
            }
        }        
         else {
            std::cout << "Received message: " << static_cast<const char *>(msg->payload()) << std::endl;
        }
        delete msg;
    }

    consumer->close();
    delete consumer;
    delete conf;

    return 0;
}