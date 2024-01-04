cmake_minimum_required(VERSION 3.10)
project(tests)
set(CMAKE_CXX_STANDARD 17)

find_package(Catch2 3 REQUIRED)

add_executable(tests 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../htk.cpp
    tests.cpp
)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(tests PRIVATE Catch2::Catch2)

