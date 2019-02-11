cmake_minimum_required (VERSION 2.6)
project (fieldviz)
add_executable(fieldviz src/main.cpp src/curve.cpp src/quat.cpp src/vec3.cpp)
target_link_libraries(fieldviz readline)

add_definitions(-std=c++14 -Og -g)

include_directories(lib)
