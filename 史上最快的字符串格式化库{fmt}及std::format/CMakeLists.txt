cmake_minimum_required(VERSION 3.13)

project(format)

set(CMAKE_CXX_STANDARD 20)

add_executable(hello_world hello_world.cpp)
add_executable(basic basic.cpp)
add_executable(align align.cpp)
add_executable(custom custom.cpp)
add_executable(binary_tree binary_tree.cpp)


find_package(fmt CONFIG REQUIRED)
target_link_libraries(hello_world PRIVATE fmt::fmt)
target_link_libraries(basic PRIVATE fmt::fmt)
target_link_libraries(align PRIVATE fmt::fmt)
target_link_libraries(custom PRIVATE fmt::fmt)
target_link_libraries(binary_tree PRIVATE fmt::fmt)

