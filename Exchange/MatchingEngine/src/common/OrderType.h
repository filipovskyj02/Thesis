#ifndef ORDERTYPE_H
#define ORDERTYPE_H
#include <stdexcept>

enum OrderType {
    MARKET,
    LIMIT,
    CANCEL
};

static OrderType convertType(const std::string& str){
    if(str == "MARKET") return MARKET;
    if(str == "LIMIT") return LIMIT;
    if(str == "CANCEL") return CANCEL;
    throw std::invalid_argument("Wrong order type: " + str);
}
#endif //ORDERTYPE_H