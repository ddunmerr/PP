cmake_minimum_required(VERSION 3.16)
project(lab2_blur CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(blur
    src/main.cpp
    src/BmpIO.cpp
    src/Blur.cpp
    src/WorkSplit.cpp
)

target_include_directories(blur PRIVATE include)

if(MSVC)
    target_compile_options(blur PRIVATE /O2 /W4)
else()
    target_compile_options(blur PRIVATE -O2 -Wall -Wextra)
endif()