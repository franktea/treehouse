cmake_minimum_required(VERSION 3.20)

project(sfml_cmake)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
  sfml
  GIT_REPOSITORY "https://github.com/SFML/SFML"
  GIT_TAG master
)

FetchContent_MakeAvailable(sfml)

add_executable(sfml_cmake main.cpp)
target_link_libraries(sfml_cmake PRIVATE sfml-graphics sfml-window sfml-system)
