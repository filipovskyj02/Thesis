#ifndef ORDERTYPE_H
#define ORDERTYPE_H
#include <stdexcept>

enum OrderType {
    MARKET,
    LIMIT
};

static OrderType convertType(const std::string& str){
    if(str == "MARKET") return MARKET;
    if(str == "LIMIT") return LIMIT;
    throw std::invalid_argument("Wrong order type: " + str);
}
#endif //ORDERTYPE_H