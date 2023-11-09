include_directories(
    ${CMAKE_SOURCE_DIR}/threadpool/include/
)

set(SRCS
    src/pool.cpp
)

add_library(threadpool STATIC ${SRCS})