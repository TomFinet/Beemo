CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(beemo CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_BUILD_TYPE Debug)

if(WIN32)
    add_compile_definitions(NOMINMAX=1)
endif(WIN32)

#add_compile_options(-fsanitize=address -fno-omit-frame-pointer -O1)
#add_link_options(-fsanitize=address)

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
include(GoogleTest)

FetchContent_Declare(
  spdlog
  URL https://github.com/gabime/spdlog/archive/refs/heads/v1.x.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(spdlog)

include_directories(
    ${CMAKE_SOURCE_DIR}/uri/include/
    ${CMAKE_SOURCE_DIR}/transport/include/
    ${CMAKE_SOURCE_DIR}/http/include/
    ${CMAKE_SOURCE_DIR}/threadpool/include/
    ${CMAKE_SOURCE_DIR}/utils/include/
)

add_subdirectory(utils)
add_subdirectory(threadpool)
add_subdirectory(transport)
add_subdirectory(uri)
add_subdirectory(http)
