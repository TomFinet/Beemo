set(LINK_LIBS spdlog)
if (LINUX)
    set(PLATFORM "linux")
elseif (WIN32)
    set(PLATFORM "win")
    set(CMAKE_WINDOWS_KITS_10_DIR "C:\\Program Files (x86)\\Windows Kits\\10")
    set(CMAKE_GENERATOR_PLATFORM  10)
    set(LINK_LIBS ${LINK_LIBS} wsock32 ws2_32)
endif ()

set(SRCS
    src/${PLATFORM}/socket.cpp
    src/${PLATFORM}/acceptor.cpp
    src/${PLATFORM}/io_ctx.cpp
    src/${PLATFORM}/server.cpp
    src/${PLATFORM}/${PLATFORM}_conn.cpp
)

if (LINUX)
    set(SRCS ${SRCS} src/${PLATFORM}/epoll_ctx.cpp)
endif (LINUX)

add_library(transport STATIC ${SRCS})
target_link_libraries(transport ${LINK_LIBS})