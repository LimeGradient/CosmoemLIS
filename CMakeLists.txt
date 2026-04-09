cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
project(LISMultiplayer VERSION 1.0.0 LANGUAGES CXX)

option(LIS_BUILD_SERVER "Build LIS Multiplayer Server" ON)
option(LIS_BUILD_CLIENT "Build LIS Multiplayer Client" ON)

include(FetchContent)
FetchContent_Declare(
    IXWebSocket
    GIT_REPOSITORY https://github.com/machinezone/IXWebSocket
    GIT_TAG v11.4.6
)
set(USE_ZLIB OFF)
FetchContent_MakeAvailable(IXWebSocket)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.92.7
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp

)

target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})

add_library(LISMultiplayerCommon STATIC
    common/Packet.cpp
    common/StringExtras.cpp
)
target_include_directories(LISMultiplayerCommon PUBLIC common/include/)
target_link_libraries(LISMultiplayerCommon PRIVATE nlohmann_json::nlohmann_json)

if (LIS_BUILD_CLIENT)
    add_subdirectory(client/)
endif()

if (LIS_BUILD_SERVER)
    add_subdirectory(server/)
endif()