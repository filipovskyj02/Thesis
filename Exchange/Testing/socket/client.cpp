#include <boost/asio.hpp>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io_context;

        // 1) Resolve & connect
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "9000");
        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        // 2) AUTH handshake
        const std::string auth_line = "AUTH 123\n";
        boost::asio::write(socket, boost::asio::buffer(auth_line));

        // 3) Read back the AUTH_OK
        boost::asio::streambuf auth_response;
        boost::asio::read_until(socket, auth_response, '\n');
        {
          std::istream is(&auth_response);
          std::string reply;
          std::getline(is, reply);
          if (reply != "OK,AUTH_OK") {
            std::cerr << "Auth failed: " << reply << "\n";
            return 1;
          }
          std::cout << "Authenticated successfully.\n";
        }
        const std::string order = "1,AAPL,0,120.5,1\n";
        // 4) Now send a few example orders
        //    (ticker,side,price,volume) or (ticker,side,volume) or (ticker,targetId)
       //const std::vector<std::string> orders = {
         // "1,AAPL,0,120.5,10\n",    // limit buy
          //"2,GOOG,1,5\n",           // market sell
          //"3,AAPL,abcd-1234-uuid\n" // cancel
        //};

        for (int i = 0; i < 100000; i++) {
            boost::asio::write(socket, boost::asio::buffer(order));

             //read back “OK,<orderId>” or “ERR,...”
            boost::asio::streambuf resp;
            boost::asio::read_until(socket, resp, '\n');
            std::istream rs(&resp);
            std::string line;
            std::getline(rs, line);
            std::cout << "Server replied: " << line << "\n";
        }
    }
    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
    return 0;
}