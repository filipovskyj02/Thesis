cmake_minimum_required(VERSION 3.10)
project(Cpp_reciever)

set(CMAKE_CXX_STANDARD 17)
find_package(Crow)
add_executable(Cpp_reciever main.cpp)
target_link_libraries(Cpp_reciever PUBLIC Crow::Crow)


