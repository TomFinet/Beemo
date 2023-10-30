CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(http CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(URI_PATH ${CMAKE_SOURCE_DIR}/../uri/)
set(SOCKPP_PATH ${CMAKE_SOURCE_DIR}/../sockpp)
set(THREADPOOL_PATH ${CMAKE_SOURCE_DIR}/../threadpool)

include_directories(
  ${CMAKE_SOURCE_DIR}/include/
  ${SOCKPP_PATH}/include/
  ${THREADPOOL_PATH}/include/
  ${URI_PATH}/include/
)

set(SRCS
    src/req_parser.cpp
    src/req_worker.cpp
    src/msg.cpp
    src/server.cpp
)

add_executable(http_main ${SRCS} src/main.cpp)
target_link_libraries(http_main
    wsock32
    ws2_32
    "${SOCKPP_PATH}/build/lib/Debug/sockpp.lib"
    "${THREADPOOL_PATH}/build/lib/Debug/threadpool.lib"
    "${URI_PATH}/build/lib/Debug/uri.lib"
)

add_library(http STATIC ${SRCS})
target_link_libraries(
    http 
    wsock32
    ws2_32
    "${SOCKPP_PATH}/build/lib/Debug/sockpp.lib"
    "${THREADPOOL_PATH}/build/lib/Debug/threadpool.lib"
    "${URI_PATH}/build/lib/Debug/uri.lib"
)


#[[include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(http_tests test/test_lexer.cpp test/test_http.cpp)
target_link_libraries(http_tests GTest::gtest_main ${LIBRARY_OUTPUT_PATH}/Debug/http.lib)

include(GoogleTest)
gtest_discover_tests(http_tests)]]