set(CMAKE_WINDOWS_KITS_10_DIR "C:\\Program Files (x86)\\Windows Kits\\10")
set(CMAKE_GENERATOR_PLATFORM  10)

set(SRCS
    src/socket.cpp
    src/acceptor.cpp
)

add_library(sockpp STATIC ${SRCS})
target_link_libraries(sockpp wsock32 ws2_32)