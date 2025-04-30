#include <boost/asio.hpp>
#include <iostream>
#include <vector>

int main() {
    namespace asio = boost::asio;
    using asio::ip::udp;

    const char*        MCAST_ADDR   = "239.255.0.1";
    const unsigned short LISTEN_PORT = 30001;
    const size_t       MAX_BUF       = 64 * 1024;  // up to 64 KB

    asio::io_context   io_context;
    udp::endpoint      listen_ep(udp::v4(), LISTEN_PORT);

    // 1) Create socket, reuse & bind
    udp::socket socket(io_context);
    socket.open(listen_ep.protocol());
    socket.set_option(asio::socket_base::reuse_address(true));
    socket.bind(listen_ep);

    // 2) Join the multicast group
    socket.set_option(
        asio::ip::multicast::join_group(
            asio::ip::make_address(MCAST_ADDR)
        )
    );

    std::cout << "Listening for multicast on "
              << MCAST_ADDR << ":" << LISTEN_PORT << "\n";

    std::vector<char> buffer(MAX_BUF);
    while (true) {
        udp::endpoint sender;
        auto len = socket.receive_from(
            asio::buffer(buffer), sender
        );
        // Print raw datagram (may contain multiple lines)
        std::cout.write(buffer.data(), len);
        std::cout.flush();
    }

    return 0;
}

