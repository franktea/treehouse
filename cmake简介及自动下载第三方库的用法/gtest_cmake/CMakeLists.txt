cmake_minimum_required(VERSION 3.20)

project(gtest_cmake)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        master
)

FetchContent_MakeAvailable(googletest)

add_executable(gtest_cmake test.cpp)
target_link_libraries(gtest_cmake gtest gtest_main)