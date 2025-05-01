
#ifndef UTILS_H
#define UTILS_H


#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

inline std::string formatTimestamp(
    const std::chrono::system_clock::time_point& tp)
{
    using namespace std::chrono;
    auto t     = system_clock::to_time_t(tp);
    auto micros = duration_cast<microseconds>(tp.time_since_epoch()) % 1000000;

    std::tm tm = *std::localtime(&t);
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S")
       << '.' << std::setw(6) << std::setfill('0') << micros.count();
    return ss.str();
}
#endif //UTILS_H
