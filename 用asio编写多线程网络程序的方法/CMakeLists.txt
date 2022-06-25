cmake_minimum_required(VERSION 3.20)

project(asio_threads)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_VERBOSE_MAKEFILE ON)

include(FetchContent)

FetchContent_Declare(asio
  GIT_REPOSITORY    https://github.com/chriskohlhoff/asio.git
  GIT_TAG master
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
)
FetchContent_Populate(asio)
include_directories(${asio_SOURCE_DIR}/asio/include/)

FetchContent_Declare(fmt
  GIT_REPOSITORY    https://github.com/fmtlib/fmt.git
  GIT_TAG master
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
)
FetchContent_Populate(fmt)
include_directories(${fmt_SOURCE_DIR}/include/)

add_executable(simple_pool simple_pool.cpp)
add_executable(coroutine coroutine.cpp)
add_executable(echo_one_thread echo_one_thread.cpp)
add_executable(echo_multi_thread echo_multi_thread.cpp)
