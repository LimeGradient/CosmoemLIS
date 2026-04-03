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

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_compile_definitions(WIN32_LEAN_AND_MEAN=1)
endif()

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