project(l2features)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
add_executable(l2features main.cpp features.cpp) 