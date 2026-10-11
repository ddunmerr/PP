cmake_minimum_required(VERSION 3.16)

project(lab1 LANGUAGES CXX)

# Стандарт C++20 обязателен (нужен std::jthread)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# По умолчанию — Release, если не задано иное
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Потоки требуют явной линковки Threads на Linux/GCC
find_package(Threads REQUIRED)

add_executable(lab1 main.cpp)

target_link_libraries(lab1 PRIVATE Threads::Threads)

# Предупреждения
if(MSVC)
    target_compile_options(lab1 PRIVATE /W4 /permissive-)
else()
    target_compile_options(lab1 PRIVATE -Wall -Wextra -Wpedantic)
endif()