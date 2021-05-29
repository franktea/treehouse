cmake_minimum_required(VERSION 3.20)

project(asio_cmake)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(asio
  GIT_REPOSITORY    https://github.com/chriskohlhoff/asio.git
  GIT_TAG master
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
)
FetchContent_Populate(asio)
include_directories(${asio_SOURCE_DIR}/asio/include/)

add_executable(asio_cmake main.cpp)
