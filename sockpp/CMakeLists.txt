set(CMAKE_WINDOWS_KITS_10_DIR "C:\\Program Files (x86)\\Windows Kits\\10")
set(CMAKE_GENERATOR_PLATFORM  10)

set(PLATFORM "linux")
if (WIN32)
    set(PLATFORM "win")
endif (WIN32)


set(SRCS
    src/${PLATFORM}/socket.cpp
    src/${PLATFORM}/acceptor.cpp
    src/${PLATFORM}/io_ctx.cpp
)

add_library(sockpp STATIC ${SRCS})
target_link_libraries(sockpp wsock32 ws2_32)