add_executable(MatchingEngine main.cpp
        Order.h
        OrderBook.cpp
        OrderBook.h
        SideEnum.h
        CommonTypes.h
        OrderType.h
)

target_link_libraries(MatchingEngine
        PRIVATE
        modern-cpp-kafka-api
)
