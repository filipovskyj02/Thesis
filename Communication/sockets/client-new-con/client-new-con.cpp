#include <boost/asio.hpp>
#include <iostream>
#include <chrono>
#include <string>
#include <thread>
#include <iomanip>  // For std::fixed and std::setprecision

using boost::asio::ip::tcp;

int main() {
    try {
        const int TOTAL_ORDERS = 10000; // Use a lower number due to high overhead
        std::string message_base = "1743877066453455 BUY LIMIT 103.891 4\n";

        auto start = std::chrono::steady_clock::now();

        for (int i = 0; i < TOTAL_ORDERS; ++i) {
            boost::asio::io_context io_context;

            // Resolve and connect
            tcp::resolver resolver(io_context);
            tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "9000");
            tcp::socket socket(io_context);
            boost::asio::connect(socket, endpoints);

            // Send message
            std::string full_msg = std::to_string(i) + " " + message_base;
            boost::asio::write(socket, boost::asio::buffer(full_msg));

            // Wait for "OK" response
            boost::asio::streambuf response;
            boost::asio::read_until(socket, response, '\n');
            std::istream response_stream(&response);
            std::string reply;
            std::getline(response_stream, reply);
            if (reply != "OK") {
                std::cerr << "Unexpected response: " << reply << std::endl;
            }

            socket.shutdown(tcp::socket::shutdown_both);
            socket.close();
        }

        auto end = std::chrono::steady_clock::now();
        auto duration = end - start;

        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration - seconds);
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration - seconds - milliseconds);
        auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds - milliseconds - microseconds);

        std::cout << "Sent " << TOTAL_ORDERS << " orders in: "
                  << seconds.count() << " s, "
                  << milliseconds.count() << " ms, "
                  << microseconds.count() << " us, and "
                  << nanoseconds.count() << " ns\n";

        double total_time_sec = std::chrono::duration<double>(duration).count();
        double throughput = TOTAL_ORDERS / total_time_sec;
        std::cout << "Throughput: " << std::fixed << std::setprecision(0)
                  << throughput << " orders/sec\n";

    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}