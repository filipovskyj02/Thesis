#ifndef DISTRIBUTOR_H
#define DISTRIBUTOR_H

#include <utility>
#include <boost/asio.hpp>
#include "../../common/SafeQueue.h"
#include "DisseminationEvent.h"
#include <thread>
#include <atomic>



class Distributor {
public:
    Distributor(boost::asio::io_context& ioContext,
                const std::string& multicastAddress,
                uint16_t multicastPort,
                SafeQueue<DisseminationEvent>& eventQueue);

    ~Distributor();

    // Launch the distributor thread
    void Start();

    // Signal the thread to stop and join it
    void Stop();

private:
    // Thread entry point: pop events and send them
    void Run();
    boost::asio::io_context& ioContext;
    std::string             multicastAddress;
    uint16_t                multicastPort;
    SafeQueue<DisseminationEvent>& eventQueue;
    std::thread             distributorThread;
    std::atomic<bool>       running{false};
};

#endif //DISTRIBUTOR_H
