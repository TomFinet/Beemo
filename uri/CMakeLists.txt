set(SRCS
  src/uri_parser.cpp
)

add_library(uri STATIC ${SRCS})

add_executable(uri_tests test/test_uri.cpp)
target_link_libraries(uri_tests GTest::gtest_main uri)
gtest_discover_tests(uri_tests)