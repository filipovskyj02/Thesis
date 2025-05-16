#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <vector>

int main() {
    namespace asio = boost::asio;
    using asio::ip::udp;
    using asio::ip::tcp;
    using Clock = std::chrono::steady_clock;

    const char* MCAST_ADDR = "239.255.0.1";
    const unsigned short MCAST_PORT = 30001;
    const char* TCP_HOST = "127.0.0.1";
    const char* TCP_PORT = "9000";
    const std::string AUTH_LINE = "AUTH 123\n";
    const std::string ORDER_LINE = "1,AAPL,0,150,5,\n";

    asio::io_context io;

    // join the UDP multicast group
    udp::socket mcast_sock(io);
    mcast_sock.open(udp::v4());
    mcast_sock.set_option(asio::socket_base::reuse_address(true));
    mcast_sock.bind(udp::endpoint(udp::v4(), MCAST_PORT));
    mcast_sock.set_option(
        asio::ip::multicast::join_group(
            asio::ip::make_address(MCAST_ADDR)
        )
    );

    // Connect to the TCP order server and authenticate
    tcp::resolver resolver(io);
    auto endpoints = resolver.resolve(TCP_HOST, TCP_PORT);
    tcp::socket tcp_sock(io);
    asio::connect(tcp_sock, endpoints);

    // send AUTH and wait for OK
    asio::write(tcp_sock, asio::buffer(AUTH_LINE));
    asio::streambuf auth_buf;
    asio::read_until(tcp_sock, auth_buf, "\n");
    {
        std::istream is(&auth_buf);
        std::string reply;
        std::getline(is, reply);
        if (reply != "OK,AUTH_OK") {
            std::cerr << "Authentication failed: " << reply << "\n";
            return 1;
        }
    }

    // Flush any pending UDP datagrams
    std::vector<char> udp_buf(64*1024);
    mcast_sock.non_blocking(true);
    while (true) {
        boost::system::error_code ec;
        mcast_sock.receive(asio::buffer(udp_buf), 0, ec);
        if (ec == asio::error::would_block) break;
    }
    mcast_sock.non_blocking(false);
    // ---------------------------------------------------------------
    // Start timer
    auto t0 = Clock::now();
    // ---------------------------------------------------------------

     // Send one order
    asio::write(tcp_sock, asio::buffer(ORDER_LINE));

    // Wait for "ACK <orderId>\n" from the server
    asio::streambuf ackBuf;
    asio::read_until(tcp_sock, ackBuf, "\n");
    std::istream is(&ackBuf);
    std::string ackLine;
    std::getline(is, ackLine);
    if (!ackLine.rfind("ACK ",0)==0){
       std::cerr << "Order confirmation failed: " << "\n";
    }

    // Block until first UDP multicast arrives
    udp::endpoint sender_ep;
    size_t len = mcast_sock.receive_from(
        asio::buffer(udp_buf), sender_ep
    );
    auto t1 = Clock::now();

    double ms = std::chrono::duration<double,std::milli>(t1 - t0).count();
    std::string data(udp_buf.data(), len);
    std::cout << "Multicast received: " << data;
    std::cout << "Multicast latency: " << ms << " ms\n";

    return 0;
}

