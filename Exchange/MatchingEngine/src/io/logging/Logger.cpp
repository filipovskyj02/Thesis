
#include "Logger.h"

#include <iostream>
#include <utility>
#include <filesystem>
#include <fstream>

Logger::Logger(std::string baseDir,
               SafeQueue<LogEvent> &eventQueue)
    : baseDir(std::move(baseDir)), eventQueue(eventQueue) {
}

Logger::~Logger() {
    stop();
}


void Logger::start() {
    if (running.exchange(true)) {
        return;
    }
    loggingThread = std::thread(&Logger::run, this);
}

void Logger::stop() {
    if (running.exchange(false)) {
        return;
    }
    eventQueue.close();
    if (loggingThread.joinable()) {
        loggingThread.join();
    }
}

void Logger::run() {
    while (running) {
        auto opt = eventQueue.pop();
        if (!opt) {
            break;
        }
        const LogEvent& e = *opt;
        const std::string& ticker = e.ticker;

        auto it = files.find(ticker);
        if (it == files.end()) {
            std::filesystem::create_directories(baseDir);
            std::ofstream ofs(baseDir + "/" + ticker + ".log",
                              std::ios::app);
            if (!ofs) {
                std::cerr << "Logger: failed to open log for "
                          << ticker << "\n";
                continue;
            }
            auto [ins, _] = files.emplace(ticker, std::move(ofs));
            it = ins;
        }
        auto& ofs = it->second;
        ofs << e << std::endl;
    }

    for (auto& [_, ofs] : files) {
        ofs.close();
    }
}


