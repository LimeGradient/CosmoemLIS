cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(LISMultiplayerHost)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/lib")

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.hpp)
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

include(FetchContent)
FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/tsudakageyu/minhook
    GIT_TAG v1.3.4
)
FetchContent_MakeAvailable(minhook)

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
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
    minhook 
    imgui 
    ixwebsocket
    nlohmann_json::nlohmann_json
    LISMultiplayerCommon
    d3d11.lib  
    dxgi.lib  
    d3dcompiler.lib
)
target_include_directories(${PROJECT_NAME} PRIVATE include/)

file(COPY "${CMAKE_BINARY_DIR}/server/inject/LISMultiplayerHost.dll" DESTINATION "${CMAKE_SOURCE_DIR}/bin/")