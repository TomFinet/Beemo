set(SRCS
    src/parser.cpp
    src/encoding.cpp
    src/err.cpp
    src/routing.cpp
    src/msg.cpp
    src/server.cpp
    src/conn.cpp
    src/pool.cpp
)

if (LINUX)
    set(SRCS ${SRCS} ${CMAKE_SOURCE_DIR}/linux/ev_epoll.cpp)
elseif (WIN32)
    set(SRCS ${SRCS} ${CMAKE_SOURCE_DIR}/windows/ev_iocp.cpp)
endif ()

add_library(http STATIC ${SRCS})
target_link_libraries(http 
    utils
    transport
    uri
    spdlog
)

add_executable(server test/main.cpp)
target_link_libraries(server http)

#add_executable(http_tests test/test_http.cpp)
#target_link_libraries(http_tests GTest::gtest_main http)
#gtest_discover_tests(http_tests)

#add_executable(client_server_tests test/test_server.cpp)
#target_link_libraries(client_server_tests http)