CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(sockpp CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_WINDOWS_KITS_10_DIR "C:\\Program Files (x86)\\Windows Kits\\10")
set(CMAKE_GENERATOR_PLATFORM  10)


include_directories(${CMAKE_SOURCE_DIR}/include/)

set(SRCS
    src/socket.cpp
    src/acceptor.cpp
    test/test.cpp
)

add_executable(sockpp ${SRCS})
target_link_libraries(sockpp wsock32 ws2_32)