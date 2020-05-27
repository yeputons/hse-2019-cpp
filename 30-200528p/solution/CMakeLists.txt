cmake_minimum_required(VERSION 3.0)
project(solution)

set(CMAKE_CXX_STANDARD 17)

add_executable(01_make_unique 01_test_make_unique.cpp doctest_main.cpp)
add_executable(02_concat 02_test_concat.cpp doctest_main.cpp)
