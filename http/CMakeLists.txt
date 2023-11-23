set(SRCS
    src/http_parser.cpp
    src/http_encoding.cpp
    src/http_err.cpp
    src/http_method.cpp
    src/routing.cpp
    src/msg.cpp
    src/server.cpp
)

add_library(http STATIC ${SRCS})
target_link_libraries(http 
    utils
    sockpp
    threadpool
    uri
    spdlog
)

if(WIN32)
    add_compile_definitions(NOMINMAX=1)
endif(WIN32)

add_executable(server src/main.cpp)
target_link_libraries(server http)

add_executable(http_tests test/test_http.cpp)
target_link_libraries(http_tests GTest::gtest_main http)
gtest_discover_tests(http_tests)