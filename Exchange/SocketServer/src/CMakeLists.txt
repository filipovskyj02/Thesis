find_package(Threads)
find_package(ZLIB)
find_package(OpenSSL)

if(OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
endif()

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

if (MSVC)
    set(Boost_USE_STATIC_LIBS "On")
    find_package( Boost 1.70 COMPONENTS system thread regex REQUIRED )
else()
    find_package( Boost 1.70 COMPONENTS system thread REQUIRED )
endif()

include_directories(${Boost_INCLUDE_DIR})

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

include_directories("${PROJECT_INCLUDE_DIR}")
add_executable(SocketServer
main.cpp
        kafka/OrderSender.cpp
        kafka/OrderSender.h
        entities/LimitOrderReq.h
        entities/MarketOrderReq.h
        entities/CancelOrderReq.h
        entities/parsers/RequestToOrder.h
        service/OrderService.h
        entities/parsers/OrderToCsv.h)
target_link_libraries(SocketServer
        PRIVATE
        modern-cpp-kafka-api)