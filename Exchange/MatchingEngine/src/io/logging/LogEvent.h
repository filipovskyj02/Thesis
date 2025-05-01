#ifndef LOGEVENT_H
#define LOGEVENT_H
#include <chrono>
#include <memory>
#include <string>
#include "../../common/Order.h"
#include "../../common/Utils.h"

struct LogEvent {
    std::string   ticker;
    std::shared_ptr<Order> order;
    std::chrono::system_clock::time_point timestamp;
};


inline std::ostream& operator<<(std::ostream& os, const LogEvent& e) {
    os
      << "[" << formatTimestamp(e.timestamp) << "]"
      << "::"
      << *e.order;
    return os;
}
#endif //LOGEVENT_H
