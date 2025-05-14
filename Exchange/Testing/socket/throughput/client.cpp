#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

using boost::asio::ip::tcp;
using HiClock = std::chrono::high_resolution_clock;

constexpr const char* HOST      = "127.0.0.1";
constexpr const char* PORT      = "9000";
constexpr const char* TICKERS[] = { "AAPL", "GOOG" };

void spammer(int id, int ordersPerThread) {
    try {
        boost::asio::io_context io;
        tcp::resolver   resolver(io);
        tcp::socket     sock(io);
        boost::asio::connect(sock, resolver.resolve(HOST, PORT));

        // 1) Mock authentication
        std::string auth = "AUTH " + std::to_string(1000 + id) + "\n";
        boost::asio::write(sock, boost::asio::buffer(auth));
        boost::asio::streambuf rb;
        boost::asio::read_until(sock, rb, "\n");  // expect "OK,AUTH_OK\n"
        rb.consume(rb.size());

        // RNG for deciding order type and fields
        //std::mt19937 gen(std::random_device{}());
        //std::uniform_real_distribution<> prob(0.0, 1.0);
        //std::uniform_int_distribution<int> pickTicker(0, 1);
        //std::uniform_int_distribution<int> pickSide(0, 1);
        //std::uniform_real_distribution<> pickPrice(100.0, 150.0);
        //std::uniform_int_distribution<int> pickVol(1, 10);
        //
        //// Store only limit orders for cancellation
        //std::vector<std::pair<std::string,std::string>> orderHistory;
        //orderHistory.reserve(ordersPerThread / 2);

        for (int n = 0; n < ordersPerThread; ++n) {
            //double r = prob(gen);
            std::string msg = "1,AAPL,0,100,1\n";
            //bool isLimit = false;
            //int ti = pickTicker(gen);

            //if (r < 0.5) {
            //    // LIMIT order (50%)
            //    int side = pickSide(gen);
            //    double px = pickPrice(gen);
            //    int vol  = pickVol(gen);
            //    msg = "1," + std::string(TICKERS[ti]) + "," +
            //          std::to_string(side) + "," +
            //          std::to_string(px)  + "," +
            //          std::to_string(vol) + "\n";
            //    isLimit = true;
            //
            //} else if (r < 0.8) {
            //    // MARKET order (30%)
            //    int side = pickSide(gen);
            //    int vol  = pickVol(gen);
            //    msg = "2," + std::string(TICKERS[ti]) + "," +
            //          std::to_string(side) + "," +
            //          std::to_string(vol) + "\n";
            //
            //} else {
            //    // CANCEL order (20%)
            //    // only if we have a limit order to cancel
            //    if (orderHistory.empty()) {
            //        --n;  // retry this iteration
            //        continue;
            //    }
            //    // pick a random stored limit order
            //    std::uniform_int_distribution<size_t> pickIdx(0, orderHistory.size() - 1);
            //    size_t idx = pickIdx(gen);
            //    auto [ticker, oid] = orderHistory[idx];
            //    // remove it from history
            //    orderHistory.erase(orderHistory.begin() + idx);
            //    msg = "3," + ticker + "," + oid + "\n";
            //}

            // send message
            boost::asio::write(sock, boost::asio::buffer(msg));

            // wait for "ACK <orderId>\n"
            boost::asio::read_until(sock, rb, "\n");
            std::istream is(&rb);
            std::string reply;
            std::getline(is, reply);
            rb.consume(rb.size());

            // if it was a limit order, record its orderId for potential cancel
           // if (isLimit && reply.rfind("ACK ", 0) == 0) {
           //     std::string oid = reply.substr(4);
           //     orderHistory.emplace_back(TICKERS[ti], oid);
           // }
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Thread " << id << " error: " << ex.what() << "\n";
    }
}

int main(int argc, char* argv[]) {
    int threads         = (argc > 1) ? std::atoi(argv[1]) : 5;
    int ordersPerThread = (argc > 2) ? std::atoi(argv[2]) : 100000;

    std::vector<std::thread> pool;
    auto t0 = HiClock::now();

    for (int i = 0; i < threads; ++i)
        pool.emplace_back(spammer, i, ordersPerThread);

    for (auto& th : pool)
        th.join();

    auto t1  = HiClock::now();
    double dur = std::chrono::duration<double>(t1 - t0).count();
    int totalOrders = threads * ordersPerThread;

    std::cout << "\nThreads   : " << threads
              << "\nOrders    : " << totalOrders
              << "\nElapsed   : " << dur << " s"
              << "\nThroughput: " << static_cast<long long>(totalOrders / dur)
              << " TPS\n";
    return 0;
}