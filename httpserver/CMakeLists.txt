CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(httpserver CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(HTTPARSER_PATH ${CMAKE_SOURCE_DIR}/../httparser)
set(SOCKPP_PATH ${CMAKE_SOURCE_DIR}/../sockpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/include/
    ${HTTPARSER_PATH}/include/
    ${SOCKPP_PATH}/include/
)

set(SRCS
    src/main.cpp
)

add_executable(httpserver $SRCS)
target_link_libraries(
    httpserver 
    ${HTTPARSER_PATH}/build/lib/httparser.lib
    ${SOCKPP_PATH}/build/lib/sockpp.lib
)