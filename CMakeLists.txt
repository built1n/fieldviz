cmake_minimum_required (VERSION 2.6)
project (fieldviz)
add_executable(fieldviz src/main.cpp)

add_definitions(-std=c++14 -O2 -g)

target_link_libraries(fieldviz fml readline)

include_directories(lib)
