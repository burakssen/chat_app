cmake_minimum_required(VERSION 3.10)
project(chatapp)

set(CMAKE_CXX_STANDARD 20)

set(SERVER_SRC src/server)
set(CLIENT_SRC src/client)
set(COMMON_SRC src/common)

set(VENDOR_SRC vendor)
set(RAYLIB ${VENDOR_SRC}/raylib)
set(RAYLIB_SRC ${RAYLIB}/src)
set(IMGUI ${VENDOR_SRC}/imgui)
set(IMGUI_BACKEND ${IMGUI}/backends)
set(IMGUI_SRC ${IMGUI})
set(RLIMGUI ${VENDOR_SRC}/rlImGui)
set(RLIMGUI_SRC ${RLIMGUI})

add_subdirectory(${RAYLIB})

# use glob to get the list of all source files for server
file(GLOB_RECURSE SERVER_SRCS
    ${SERVER_SRC}/*.cpp
    ${COMMON_SRC}/*.cpp
)

# use glob to get the list of all source files for client
file(GLOB_RECURSE CLIENT_SRCS
    ${CLIENT_SRC}/*.cpp
    ${COMMON_SRC}/*.cpp
    ${RLIMGUI}/rlImGui.cpp
    ${RLIMGUI}/*.h
    ${IMGUI_SRC}/imgui_demo.cpp
    ${IMGUI_SRC}/imgui_draw.cpp
    ${IMGUI_SRC}/imgui_tables.cpp
    ${IMGUI_SRC}/imgui_widgets.cpp
    ${IMGUI_SRC}/imgui.cpp
    ${IMGUI_SRC}/misc/cpp/imgui_stdlib.cpp
)

# Add the server executable
add_executable(server ${SERVER_SRCS})

# Add the client executable
add_executable(client ${CLIENT_SRCS})

target_include_directories(server PUBLIC
    ${SERVER_SRC}
    ${COMMON_SRC}
)

target_include_directories(client PUBLIC
    ${CLIENT_SRC}
    ${COMMON_SRC}
    ${RAYLIB_SRC} 
    ${IMGUI_SRC}
    ${RLIMGUI_SRC}
    ${IMGUI_BACKEND}
    ${IMGUI_SRC}/misc/cpp
)


target_link_libraries(client PUBLIC
    raylib
    "-framework OpenGL"
    "-framework IOKit"
    "-framework Cocoa"
)