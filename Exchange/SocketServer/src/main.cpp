#include <utility>
#include <boost/asio.hpp>
#include <iostream>
#include "kafka/OrderSender.h"
#include "service/OrderService.h"

using boost::asio::ip::tcp;


int main() {
    boost::asio::io_context ioc{1};

    OrderSender sender("orders");
    OrderService service(sender);

    tcp::acceptor acceptor{ioc, {tcp::v4(), 9000}};
    std::cout << "TCP order server listening on port 9000\n";

    for (;;) {
        tcp::socket sock{ioc};
        acceptor.accept(sock);

        std::thread{
            [s = std::move(sock), &service]() mutable {
                try {
                    boost::asio::streambuf buf;
                    std::istream is(&buf);

                    // expect AUTH line first
                    boost::asio::read_until(s, buf, '\n');
                    std::string auth;
                    std::getline(is, auth);
                    if (auth.rfind("AUTH ", 0) != 0) {
                        boost::asio::write(s, boost::asio::buffer("ERR,auth failed\n"));
                        return;
                    }
                    int userId = std::stoi(auth.substr(5));
                    std::cout << "User ID: " << userId << " succesfully authed." << std::endl;
                    boost::asio::write(s, boost::asio::buffer("OK,AUTH_OK\n"));

                    // thereafter each line is a CSV order
                    while (true) {
                        boost::asio::read_until(s, buf, '\n');
                        std::string line;
                        std::getline(is, line);
                        if (line.empty()) break;

                        auto reply = service.processCsvLine(line, userId) + "\n";
                        boost::asio::write(s, boost::asio::buffer(reply));
                    }
                } catch (...) {
                }
            }
        }.detach();
    }
}

