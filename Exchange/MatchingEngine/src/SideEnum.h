#ifndef SIDEENUM_H
#define SIDEENUM_H
#include <stdexcept>

enum Side {
    BUY,
    SELL
};

static Side convertSide(const std::string& str){
    if(str == "BUY") return BUY;
    if(str == "SELL") return SELL;
    throw std::invalid_argument("Invalid Side " + str);
}
#endif //SIDEENUM_H
