cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(V8_Engine VERSION 1.0.0 LANGUAGES CXX)

add_subdirectory(Engine/src/Core)
add_subdirectory(Engine/src/Renderer)

set(ENTRY_POINT Engine/main.cpp)

add_executable(V8 ${ENTRY_POINT})

target_include_directories(V8 PRIVATE ${CMAKE_SOURCE_DIR}/Engine/include)
target_link_directories(V8 PRIVATE ${CMAKE_SOURCE_DIR}/build/lib)
target_link_libraries(V8 PRIVATE Core Renderer)
