cmake_minimum_required(VERSION 3.10)
project(oj_problem_033 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(code main.cpp)
target_include_directories(code PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

