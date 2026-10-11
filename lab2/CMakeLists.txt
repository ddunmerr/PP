cmake_minimum_required(VERSION 3.16)
project(blur CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(blur
    src/main.cpp
    src/image.cpp
    src/blur.cpp
)

target_compile_options(blur PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-O2 -Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/O2 /W4>
)

# stb — header-only, реализацию включаем ровно в одном TU (image.cpp)
target_include_directories(blur PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})