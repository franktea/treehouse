cmake_minimum_required(VERSION 3.20)

project(quadtree)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
  sfml
  GIT_REPOSITORY "https://github.com/SFML/SFML"
  GIT_TAG master
)

FetchContent_MakeAvailable(sfml)

add_executable(quadtree_test quadtree_test.cpp)
target_link_libraries(quadtree_test PRIVATE sfml-graphics sfml-window sfml-system)

set_target_properties(quadtree_test
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)