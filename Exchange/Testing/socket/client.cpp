#include <boost/asio.hpp>
#include <atomic>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

using boost::asio::ip::tcp;
using HiClock = std::chrono::high_resolution_clock;          // safe alias

constexpr const char* HOST = "127.0.0.1";
constexpr const char* PORT = "9000";
constexpr const char* TICKERS[2] = {"AAPL", "GOOG"};



void spammer(int id, int ordersPerThread)
{
    try {
        boost::asio::io_context io;
        tcp::resolver  res(io);
        tcp::socket    sock(io);
        boost::asio::connect(sock, res.resolve(HOST, PORT));

        // auth
        std::string auth = "AUTH " + std::to_string(123 + id) + "\n";
        boost::asio::write(sock, boost::asio::buffer(auth));
        boost::asio::streambuf rb;
        boost::asio::read_until(sock, rb, '\n');        // expect OK,AUTH_OK

        // RNG per thread
        //std::mt19937 gen(std::random_device{}());
        //std::uniform_int_distribution<int> pick(0, 1);
        //std::uniform_real_distribution<>  px (100.0, 150.0);
        //std::uniform_int_distribution<int> vol(1, 10);

        for (int n = 0; n < ordersPerThread; ++n) {
            //std::string msg = "1," + std::string(TICKERS[pick(gen)]) + ',' +
              //                std::to_string(pick(gen)) + ',' +
                //              std::to_string(px(gen))   + ',' +
                  //            std::to_string(vol(gen))  + '\n';

            std::string testMsg = "1,AAPL,0,100,1,\n";
            boost::asio::write(sock, boost::asio::buffer(testMsg));

            // read server reply to guarantee acceptance
            rb.consume(rb.size());
            boost::asio::read_until(sock, rb, '\n');


        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Thread " << id << " error: " << ex.what() << '\n';
    }
}

int main(int argc, char* argv[])
{
    int threads         = (argc > 1) ? std::atoi(argv[1]) : 8;
    int ordersPerThread = (argc > 2) ? std::atoi(argv[2]) : 100'000;


    std::vector<std::thread> pool;
    auto t0 = HiClock::now();

    for (int i = 0; i < threads; ++i)
        pool.emplace_back(spammer, i, ordersPerThread);

    for (auto& th : pool) th.join();

    auto t1  = HiClock::now();
    auto dur = std::chrono::duration<double>(t1 - t0).count();
    int orderCnt = threads * ordersPerThread;

    std::cout << "\nThreads  : " << threads
              << "\nOrders   : " << orderCnt
              << "\nElapsed  : " << dur << " s"
              << "\nThroughput: " << static_cast<long long>(orderCnt / dur)
              << " TPS\n";
}
