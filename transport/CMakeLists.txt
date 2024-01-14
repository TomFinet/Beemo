set(LINK_LIBS spdlog)
if (LINUX)
    set(PLATFORM "linux")
elseif (WIN32)
    set(PLATFORM "windows")
    set(CMAKE_WINDOWS_KITS_10_DIR "C:\\Program Files (x86)\\Windows Kits\\10")
    set(CMAKE_GENERATOR_PLATFORM  10)
    set(LINK_LIBS ${LINK_LIBS} wsock32 ws2_32)
endif ()

set(SRCS
    src/socket.cpp
    ${CMAKE_SOURCE_DIR}/${PLATFORM}/socket.cpp
    ${CMAKE_SOURCE_DIR}/${PLATFORM}/acceptor.cpp
    ${CMAKE_SOURCE_DIR}/${PLATFORM}/io_buf.cpp
)

add_library(transport STATIC ${SRCS})
target_link_libraries(transport ${LINK_LIBS})