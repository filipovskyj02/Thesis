#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

int main() {
    namespace asio = boost::asio;
    using asio::ip::udp;

    const char*        MCAST_ADDR     = "239.255.0.1";  // Administratively‑scoped multicast
    const unsigned short MCAST_PORT   = 30001;
    const int          TOTAL_PACKETS  = 1000000;
    const int          BATCH_SIZE     = 15;

    asio::io_context   io_context;
    udp::socket        socket(io_context, udp::v4());

    // 1) Set TTL so packets stay on local subnet
    socket.set_option(asio::ip::multicast::hops(1));

    // 2) Destination endpoint = multicast group + port
    udp::endpoint      dest(
        asio::ip::make_address(MCAST_ADDR),
        MCAST_PORT
    );

    std::cout << "Multicast sender → " << MCAST_ADDR
              << ":" << MCAST_PORT
              << "  ("
              << TOTAL_PACKETS << " msgs, batch=" << BATCH_SIZE
              << ")\n";

    auto t_start = std::chrono::steady_clock::now();

    // 3) Send in batches of lines per datagram
    for (int batch_i = 0; batch_i < TOTAL_PACKETS / BATCH_SIZE; ++batch_i) {
        // timestamp once per batch
        auto ts = std::chrono::duration_cast<std::chrono::microseconds>(
                      std::chrono::steady_clock::now()
                         .time_since_epoch()
                   ).count();

        std::ostringstream ss;
        for (int i = 0; i < BATCH_SIZE; ++i) {
            int id = batch_i * BATCH_SIZE + i;
            ss << id << " "
               << ts << " "
               << "SELL LIMIT 122.618 3\n";
        }

        auto msg = ss.str();
        socket.send_to(asio::buffer(msg), dest);
    }

    auto t_end = std::chrono::steady_clock::now();
    auto d = t_end - t_start;
    auto s = std::chrono::duration_cast<std::chrono::seconds>(d);
    auto m = std::chrono::duration_cast<std::chrono::milliseconds>(d - s);
    auto u = std::chrono::duration_cast<std::chrono::microseconds>(d - s - m);

    std::cout << "Sent " << TOTAL_PACKETS << " msgs in "
              << s.count() << " s, "
              << m.count() << " ms, "
              << u.count() << " us\n";

    return 0;
}
