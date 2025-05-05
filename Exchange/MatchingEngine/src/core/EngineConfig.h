#ifndef ENGINECONFIG_H
#define ENGINECONFIG_H

#include <cstdint>
#include <string>
#include <vector>

struct EngineConfig {
    std::vector<std::string> tickers;
    std::string kafkaBroker;
    std::string kafkaTopicInOrders;
    std::string kafkaTopicOutNotifications;
    std::string logDirectory;
    std::string multicastAddress;
    uint16_t multicastPort;
};

#endif //ENGINECONFIG_H
