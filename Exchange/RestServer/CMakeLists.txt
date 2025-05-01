cmake_minimum_required(VERSION 3.30)
project(RestServer)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(test)