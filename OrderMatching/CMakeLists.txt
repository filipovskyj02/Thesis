cmake_minimum_required(VERSION 3.22)
project(OrderMatching)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_COMPILER "/usr/local/gcc-14.2.0/bin/g++-14.2.0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -O3 \
    -fvisibility=hidden -flto=auto \
    -ffast-math -funroll-all-loops \
    -faggressive-loop-optimizations \
    -fomit-frame-pointer -fno-stack-protector \
    -fno-exceptions -fno-rtti \
    -fno-semantic-interposition -fipa-pta \
    -fdevirtualize-at-ltrans -fno-plt \
    -falign-functions=64 -mprefer-vector-width=512 \
    -fno-sanitize=all -fno-stack-check \
    -fno-math-errno -fno-trapping-math \
    -fstrict-aliasing -fno-check-new \
    -fdelete-null-pointer-checks \
    -fno-enforce-eh-specs \
    -fno-ident -fno-asynchronous-unwind-tables \
    -fno-unwind-tables -fno-bounds-check \
    -fno-math-errno -funsafe-loop-optimizations \
    -fsingle-precision-constant \
    -fno-threadsafe-statics \
    -fno-align-jumps \
    -fno-align-loops -fno-align-labels \
    -mno-vzeroupper -mno-red-zone \
    -mno-push-args -mno-accumulate-outgoing-args \
    -minline-all-stringops \
    -fprefetch-loop-arrays \
    -mprefer-vector-width=512 \
    -fpeel-loops \
    -funroll-loops \
    -fmodulo-sched \
    -fmodulo-sched-allow-regmoves \
    -fsched-pressure \
    -fsched-spec-load -fexceptions\
    -fno-tree-loop-distribute-patterns")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
    -Wl,--gc-sections \
    -Wl,--as-needed \
    -Wl,-O3 \
    -Wl,--strip-all \
    -Wl,--hash-style=gnu \
    -Wl,--allow-multiple-definition \
    -z noexecstack \
    -z nodlopen \
    -z noseparate-code \
    -z norelro")
set(PQ_INCLUDE_DIR "/usr/include/postgresql")
set(PQ_LIBRARY "/usr/lib/x86_64-linux-gnu/libpq.so")
file(GLOB SOURCES "*.cpp" "*.h")
include_directories(include)
add_executable(OrderMatching ${SOURCES}
        PostgresLogger.h
        PostgresLogger.cpp
        PostgresThreadedLogger.cpp
        PostgresThreadedLogger.h
        KDBLogger.cpp
        KDBLogger.h
        FileLogger.cpp
        FileLogger.h)
target_compile_definitions(OrderMatching PRIVATE
    CACHE_LINE_SIZE=64
    ASSUME_ALIGNED
    UNSAFE_OPTIMIZATIONS
    FORCE_INLINE_ALL
)
target_include_directories(OrderMatching PRIVATE ${PQ_INCLUDE_DIR})
target_link_libraries(OrderMatching PRIVATE ${PQ_LIBRARY} ${CMAKE_SOURCE_DIR}/include/c.o)