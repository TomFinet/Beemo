set(SRCS
    src/http_parser.cpp
    src/msg.cpp
    src/server.cpp
)

add_library(http STATIC ${SRCS})
target_link_libraries(http 
    wsock32
    ws2_32
    sockpp
    threadpool
    uri
)

add_executable(server src/main.cpp)
target_link_libraries(server http)

add_executable(http_tests test/test_http.cpp)
target_link_libraries(http_tests GTest::gtest_main http)
gtest_discover_tests(http_tests)