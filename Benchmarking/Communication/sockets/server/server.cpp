#include <boost/asio.hpp>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

void session(tcp::socket socket) {
    try {
        boost::asio::streambuf buffer;
        std::cout << "Session started with " << socket.remote_endpoint() << "\n";
        while (true) {
            // Read data until a newline is encountered.
            boost::asio::read_until(socket, buffer, '\n');

            // Extract the message as a string.
            std::istream is(&buffer);
            std::string message;
            std::getline(is, message);

            if (message.empty())
                continue;

            std::cout << "Received: " << message << "\n";


            std::string reply = "OK\n";
            boost::asio::write(socket, boost::asio::buffer(reply));
        }
    } catch (std::exception &e) {
        std::cerr << "Session ended: " << e.what() << "\n";
    }
}

int main() {
    try {
        boost::asio::io_context io_context;

        // Listen on port 9000.
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 9000));
        std::cout << "Persistent Server: Listening on port 9000...\n";

        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            session(std::move(socket));
        }
    } catch (std::exception &e) {
        std::cerr << "Server exception: " << e.what() << "\n";
    }
    return 0;
}
