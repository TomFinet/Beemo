CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(httparser CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

include_directories(${CMAKE_SOURCE_DIR}/include/)

set(SRCS

)

add_executable(httparser ${SRCS})
target_link_libraries(httparser)