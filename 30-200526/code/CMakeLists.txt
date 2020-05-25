cmake_minimum_required(VERSION 3.16)
project(code)

set(CMAKE_CXX_STANDARD 20)

add_executable(code main.cpp)
add_executable(02-debt-01-arrow 02-debt/01-arrow.cpp)
add_executable(02-debt-02-arrow-lifetime 02-debt/02-arrow-lifetime.cpp)
