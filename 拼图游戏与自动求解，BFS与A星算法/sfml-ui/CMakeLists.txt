cmake_minimum_required(VERSION 3.20)

project(15puzzle-ui)

set(CMAKE_CXX_STANDARD 20)

set(BUILD_SHARED_LIBS OFF)

include(FetchContent)
FetchContent_Declare(
  sfml
  GIT_REPOSITORY "https://github.com/SFML/SFML"
  GIT_TAG master
)

FetchContent_MakeAvailable(sfml)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(15puzzle 15puzzle.cpp)
target_link_libraries(15puzzle PRIVATE sfml-graphics sfml-window sfml-system)

add_executable(bfs bfs.cpp)
target_link_libraries(bfs PRIVATE sfml-graphics sfml-window sfml-system)

add_executable(astar astar.cpp)
target_link_libraries(astar PRIVATE sfml-graphics sfml-window sfml-system)