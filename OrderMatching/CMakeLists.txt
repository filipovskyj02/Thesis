cmake_minimum_required(VERSION 3.30)
project(OrderMatching)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(OrderMatching main.cpp
        SideEnum.h
        Order.h
        OrderType.h
        OrderBook.h
        CommonTypes.h
        OrderBook.cpp
)
