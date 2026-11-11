cmake_minimum_required(VERSION 3.10)
project(tarea6_arboles_grafos CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tarea6 src/main.cpp)
target_include_directories(tarea6 PRIVATE include)
target_compile_options(tarea6 PRIVATE -Wall -Wextra -pedantic)
