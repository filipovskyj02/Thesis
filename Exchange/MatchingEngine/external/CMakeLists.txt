set(CMAKE_CXX_STANDARD 17)
project(modern-cpp-kafka-api)

# add_library(${PROJECT_NAME} INTERFACE)
add_library(modern-cpp-kafka-api INTERFACE)

#target_include_directories(${PROJECT_NAME}
 #       INTERFACE
  #      ${CMAKE_CURRENT_LIST_DIR}       # so <kafka/...> resolves
#)
target_include_directories(modern-cpp-kafka-api
  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}    # now -I …/MatchingEngine/external
)
#---------------------------
# librdkafka library
#---------------------------
if (DEFINED ENV{LIBRDKAFKA_INCLUDE_DIR})
    set(LIBRDKAFKA_INCLUDE_DIR $ENV{LIBRDKAFKA_INCLUDE_DIR})
else ()
    find_file(LIBRDKAFKA_HEADER
            NAMES rdkafka.h
            HINTS /usr/include/librdkafka /usr/local/include/librdkafka /opt/homebrew/include/librdkafka)

    cmake_path(GET LIBRDKAFKA_HEADER PARENT_PATH LIBRDKAFKA_INCLUDE_DIR)
    cmake_path(GET LIBRDKAFKA_INCLUDE_DIR PARENT_PATH LIBRDKAFKA_INCLUDE_DIR)
endif ()

if (DEFINED ENV{LIBRDKAFKA_LIBRARY_DIR})
    set(LIBRDKAFKA_LIBRARY_DIR $ENV{LIBRDKAFKA_LIBRARY_DIR})
else ()
    find_library(LIBRDKAFKA_LIB
            NAMES rdkafka
            HINTS /usr/lib /usr/local/lib /opt/homebrew/lib)

    cmake_path(GET LIBRDKAFKA_LIB PARENT_PATH LIBRDKAFKA_LIBRARY_DIR)
endif ()

if (EXISTS "${LIBRDKAFKA_INCLUDE_DIR}/librdkafka/rdkafka.h")
    message(STATUS "librdkafka include directory: ${LIBRDKAFKA_INCLUDE_DIR}")
else ()
    message(FATAL_ERROR "Could not find headers for librdkafka!")
endif ()

if (EXISTS "${LIBRDKAFKA_LIBRARY_DIR}/librdkafka.a" OR EXISTS "${LIBRDKAFKA_LIBRARY_DIR}/librdkafka.so" OR EXISTS "${LIBRDKAFKA_LIBRARY_DIR}/rdkafka.lib" )
    message(STATUS "librdkafka library directory: ${LIBRDKAFKA_LIBRARY_DIR}")
else ()
    message(FATAL_ERROR "Could not find library for librdkafka!")
endif ()


#---------------------------
# pthread library (for linux only)
#---------------------------
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    find_library(PTHREAD_LIB pthread)
    if (PTHREAD_LIB)
        message(STATUS "pthread library: ${PTHREAD_LIB}")
    else ()
        message(FATAL_ERROR "Could not find library for pthread!")
    endif ()
endif ()
target_include_directories(${PROJECT_NAME} SYSTEM INTERFACE ${LIBRDKAFKA_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} INTERFACE ${LIBRDKAFKA_LIBRARY_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE rdkafka)

#---------------------------
# pthread
#---------------------------
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_link_libraries(${PROJECT_NAME} INTERFACE pthread)
endif ()

