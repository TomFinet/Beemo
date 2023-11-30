set(PLATFORM "linux")
set(LINK_LIBS spdlog)
if (WIN32)
    set(PLATFORM "win")
    set(CMAKE_WINDOWS_KITS_10_DIR "C:\\Program Files (x86)\\Windows Kits\\10")
    set(CMAKE_GENERATOR_PLATFORM  10)
    set(LINK_LIBS ${LINK_LIBS} wsock32 ws2_32)
endif (WIN32)

set(SRCS
    src/${PLATFORM}/socket.cpp
    src/${PLATFORM}/acceptor.cpp
    src/${PLATFORM}/io_ctx.cpp
    src/${PLATFORM}/server.cpp
)

add_library(transport STATIC ${SRCS})
target_link_libraries(transport ${LINK_LIBS})