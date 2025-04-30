//
// Created by JFili on 28.01.2025.
//

#ifndef ORDERTYPE_H
#define ORDERTYPE_H
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
