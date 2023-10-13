CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(httparser CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

include_directories(${CMAKE_SOURCE_DIR}/include/)

set(SRCS
    src/http_parser.cpp
)

add_library(httparser STATIC ${SRCS})
target_link_libraries(httparser)


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(httparser_tests test/test_lexer.cpp test/test_parser.cpp)
target_link_libraries(httparser_tests GTest::gtest_main ${LIBRARY_OUTPUT_PATH}/Debug/httparser.lib)

include(GoogleTest)
gtest_discover_tests(httparser_tests)