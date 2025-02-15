#include <chrono>
#include <iostream>
#include <fstream>
#include <random>
#include <set>
#include <string>
#include <format>

#define MAX_PRICE 105.0
#define MIN_PRICE 95.0
#define MAX_VOLUME 1000
#define MIN_VOLUME 1
#define LIMIT_TO_MARKET_RATIO 0.67 // https://www.researchgate.net/figure/Distribution-of-order-type-and-order-size_tbl2_5216952
#define CANCELATION_RATE 0.9 // https://www.sciencedirect.com/science/article/abs/pii/S0378426621001291 -- cannot be properly simulated as the figure refers to a longer timeline
#define INITIAL_ORDERS 30000
#define GENERATED_ORDERS 50000

enum class OrderType { MARKET, LIMIT };

enum class Side { BUY, SELL };

using Price = double;
using Volume = uint64_t;


int orderId = 1;
std::uniform_real_distribution<> zeroToOneDist(0.0, 1.0);
std::set<int> activeOrders;


std::string getTimeStamp() {
    const auto now = std::chrono::system_clock::now();
    return std::format("{:%H-%M-%d-%m-%Y}", now);
}

int getRandomIntFromSet(const std::set<int>& set) {
    return *std::next(set.begin(), rand() % set.size());
}

std::string orderTypeToString(OrderType type) {
    return (type == OrderType::MARKET) ? "MARKET" : "LIMIT";
}

std::string sideToString(Side side) {
    return (side == Side::BUY) ? "BUY" : "SELL";
}

std::ofstream openFile(const std::string &filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        throw std::runtime_error("Failed to open file");
    }
    return outFile;
}

void fillOrderBook(std::ofstream &outFile, int numOrders, std::mt19937 &gen,
                   std::uniform_int_distribution<> &volumeDist) {
    Price midPrice = MIN_PRICE + ((MAX_PRICE - MIN_PRICE) / 2);
    std::uniform_real_distribution<> priceDistBuy(MIN_PRICE, midPrice);
    std::uniform_real_distribution<> priceDistSell(midPrice, MAX_PRICE);

    for (int i = 0; i < numOrders; ++i) {
        Side side = (zeroToOneDist(gen) >= 0.5) ? Side::BUY : Side::SELL;
        Price price = side == Side::BUY ? priceDistBuy(gen) : priceDistSell(gen);
        Volume volume = volumeDist(gen);
        outFile << orderId << " " << sideToString(side) << " LIMIT " << price << " " << volume << "\n";
        activeOrders.insert(orderId);
        orderId++;
    }
}

void generateOrders(std::ofstream &outFile, int numOrders, std::mt19937 &gen,
                    std::uniform_int_distribution<> volumeDist) {
    std::uniform_real_distribution<> priceDist(MIN_PRICE, MAX_PRICE);
    for (int i = 0; i < numOrders; ++i) {
        Side side = (zeroToOneDist(gen) >= 0.5) ? Side::BUY : Side::SELL;
        Price price = priceDist(gen);
        Volume volume = volumeDist(gen);
        if (zeroToOneDist(gen) <= LIMIT_TO_MARKET_RATIO) {
            if (zeroToOneDist(gen) <= CANCELATION_RATE && !activeOrders.empty()) {
                int canceledId = getRandomIntFromSet(activeOrders);
                outFile << canceledId << " CANCEL" << "\n";
                activeOrders.erase(canceledId);
            }
            else {
                outFile << orderId << " " << sideToString(side) << " LIMIT " << price << " " << volume << "\n";
                activeOrders.insert(orderId);
            }
        }
        else {
            outFile << orderId << " " << sideToString(side) << " MARKET " << volume << "\n";
        }
        orderId++;
    }
}

int main() {
    const std::string filename = std::format("outputs/{}-{}-{}.txt", INITIAL_ORDERS, GENERATED_ORDERS, getTimeStamp());
    std::ofstream outFile = openFile(filename);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> volumeDist(MIN_VOLUME, MAX_VOLUME);
    outFile << INITIAL_ORDERS << " " << GENERATED_ORDERS << "\n";
    fillOrderBook(outFile, INITIAL_ORDERS, gen, volumeDist);
    generateOrders(outFile, GENERATED_ORDERS, gen, volumeDist);
    outFile.close();
    std::cout << "Order data written to " << filename << std::endl;
    return 0;
}
