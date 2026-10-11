cmake_minimum_required(VERSION 3.16)
project(bank LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(lab3_lib
    src/Bank.cpp
    src/Wallet.cpp
    src/Actor.cpp
    src/Director.cpp
    src/Father.cpp
    src/Mother.cpp
    src/Seller.cpp
    src/Child.cpp
    src/EconomySimulation.cpp
)
target_include_directories(lab3_lib PUBLIC include)

find_package(Threads REQUIRED)
target_link_libraries(lab3_lib PUBLIC Threads::Threads)

add_executable(bank src/main.cpp)
target_link_libraries(bank PRIVATE lab3_lib)

enable_testing()
add_executable(bank_tests tests/test_bank.cpp)
target_link_libraries(bank_tests PRIVATE lab3_lib)
add_test(NAME bank_tests COMMAND bank_tests)