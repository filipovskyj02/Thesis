#include <atomic>
#include <kafka/KafkaConsumer.h>

#include <iostream>
#include <signal.h>
#include <string>

std::atomic_bool running = {true};

void stopRunning(int sig) {
    if (sig != SIGINT) return;

    if (running) {
        running = false;
    } else {
        // Restore the signal handler, -- to avoid stuck with this handler
        signal(SIGINT, SIG_IGN); // NOLINT
    }
}

int main()
{
    using namespace kafka;
    using namespace kafka::clients::consumer;

    // Use Ctrl-C to terminate the program
    signal(SIGINT, stopRunning);    // NOLINT

    const std::string brokers = "localhost:9092";
    const Topic topic = "test";            // NOLINT

    // Prepare the configuration
    const Properties props({{"bootstrap.servers", {brokers}},
                            {"max.poll.records", {"10000"}}});

    // Create a consumer instance
    KafkaConsumer consumer(props);

    // Subscribe to topics
    consumer.subscribe({topic});
    int cnt = 0;
    auto start = std::chrono::steady_clock::now();
    while (running) {
        // Poll messages from Kafka brokers
        auto records = consumer.poll(std::chrono::milliseconds(10));

        for (const auto& record: records) {
            if (!record.error()) {
                std::cout << record.value().toString() << std::endl;

            } else {
                std::cerr << record.toString() << std::endl;
            }
        }
    }

    // No explicit close is needed, RAII will take care of it
    consumer.close();
}
