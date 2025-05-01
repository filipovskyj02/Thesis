add_executable(RestServer
main.cpp)
target_link_libraries(RestServer
        PRIVATE
        modern-cpp-kafka-api)