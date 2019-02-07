cmake_minimum_required (VERSION 2.6)
project (fieldviz)
add_executable(fieldviz main.cpp curve.cpp quat.cpp vec3.cpp)

add_definitions(-std=c++14 -O2 -g)

include_directories(lib)
