#include "Distributor.h"
#include <boost/asio/ip/multicast.hpp>
#include <iostream>

Distributor::Distributor(boost::asio::io_context &ioCtx,
                         const std::string &addr,
                         uint16_t port,
                         SafeQueue<DisseminationEvent> &queue)
    : ioContext(ioCtx)
      , multicastAddress(addr)
      , multicastPort(port)
      , eventQueue(queue) {
}

Distributor::~Distributor() {
    stop();
}

void Distributor::start() {
    if (running.exchange(true)) {
        return;
    }
    distributorThread = std::thread(&Distributor::run, this);
}

void Distributor::stop() {
    if (!running.exchange(false)) {
        return;
    }
    eventQueue.close();
    if (distributorThread.joinable()) {
        distributorThread.join();
    }
}

void Distributor::run() {
    try {
        using boost::asio::ip::udp;

        udp::socket socket(ioContext, udp::v4());
        socket.set_option(boost::asio::ip::multicast::hops(1));
        socket.set_option(boost::asio::ip::multicast::enable_loopback(true));

        udp::endpoint endpoint(
            boost::asio::ip::make_address(multicastAddress),
            multicastPort
        );

        while (running) {
            auto optEvent = eventQueue.pop();
            if (!optEvent) {
                break;
            }

            auto msg = toString(*optEvent);
            socket.send_to(boost::asio::buffer(msg), endpoint);
        }
    } catch (std::exception &ex) {
        std::cerr << "Distributor error: " << ex.what() << std::endl;
    }
}
