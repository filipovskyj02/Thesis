#ifndef LOGGER_H
#define LOGGER_H
#include <atomic>
#include <string>
#include <thread>
#include <unordered_map>
#include "../../common/SafeQueue.h"
#include "LogEvent.h"



class Logger {
public:
    explicit Logger(std::string baseDir,
                    SafeQueue<LogEvent>& eventQueue);
    ~Logger();

    void start();
    void stop();

private:
    void run();

    std::string                              baseDir;
    std::unordered_map<std::string, std::ofstream> files;
    SafeQueue<LogEvent>&                     eventQueue;
    std::thread                              loggingThread;
    std::atomic<bool>                        running{false};
};
#endif //LOGGER_H
