#include <chrono>
#include <iostream>
#include <fstream>
#include <random>
#include <set>
#include <string>
#include <iomanip>
#include <ctime>
#include <sstream>

#define MAX_PRICE 340
#define MIN_PRICE 53.0
#define MAX_VOLUME 100
#define MIN_VOLUME 1
#define LIMIT_TO_MARKET_RATIO 0.67 // https://www.researchgate.net/figure/Distribution-of-order-type-and-order-size_tbl2_5216952
#define CANCELATION_RATE 0.95 // https://www.sciencedirect.com/science/article/abs/pii/S0378426621001291 -- cannot be properly simulated as the figure refers to a longer timeline
#define INITIAL_ORDERS 1000
#define GENERATED_ORDERS 10000

enum class OrderType { MARKET, LIMIT, CANCEL };

enum class Side { BUY, SELL };

using Price = double;
using Volume = uint64_t;


int orderId = 0;
std::uniform_real_distribution<> zeroToOneDist(0.0, 1.0);
std::set<int> activeOrders;


std::string getTimeStamp() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
    return oss.str();
}

int getRandomIntFromSet(const std::set<int>& set) {
    return *std::next(set.begin(), rand() % set.size());
}

std::string orderTypeToString(OrderType type) {
    switch (type) {
        case OrderType::MARKET: return "MARKET";
        case OrderType::LIMIT: return "LIMIT";
        case OrderType::CANCEL: return "CANCEL";
        default: return "UNKNOWN";
    }
}

std::string sideToString(Side side) {
    return (side == Side::BUY) ? "BUY" : "SELL";
}

void printToStream(std::ofstream &out, int id, Side side, OrderType type, Price price, Volume volume) {
    out << id << " " << sideToString(side) << " " << orderTypeToString(type) << " " << price << " " << volume << "\n";
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
        printToStream(outFile, orderId, side, OrderType::LIMIT, price, volume);
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
                printToStream(outFile, canceledId, Side::BUY, OrderType::CANCEL, 0, 0);
                activeOrders.erase(canceledId);
            }
            else {
                printToStream(outFile, orderId, side, OrderType::LIMIT, price, volume);
                activeOrders.insert(orderId);
                orderId++;
            }
        }
        else {
            printToStream(outFile, orderId, side, OrderType::MARKET, 0, volume);
            orderId++;
        }

    }
}


int main() {
    std::string filename = "outputs/" + std::to_string(INITIAL_ORDERS) + "-" +
        std::to_string(GENERATED_ORDERS) + "-" +
        std::to_string(LIMIT_TO_MARKET_RATIO) + "-" +
        std::to_string(CANCELATION_RATE) + "-" +
        getTimeStamp() + ".txt";

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
