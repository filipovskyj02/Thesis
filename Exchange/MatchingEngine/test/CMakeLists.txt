include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)

add_executable(
        OrderBookTest
        OrderBookTest.cpp
        KafkaParserTest.cpp
)
target_link_libraries(
        OrderBookTest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(OrderBookTest)
