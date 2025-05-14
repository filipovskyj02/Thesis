cmake_minimum_required(VERSION 3.20)
project(SocketServer)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE
            STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel"
            FORCE)
endif()

# (optional) tighten your optimization flags
set(CMAKE_CXX_FLAGS_RELEASE   "${CMAKE_CXX_FLAGS_RELEASE}   -O3 -march=native")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(test)