cmake_minimum_required(VERSION 3.15)

project(socks5)

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

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master
)
FetchContent_MakeAvailable(fmt)
include_directories(${fmt_SOURCE_DIR}/include/)

#add_executable(echo echo.cpp)
add_executable(socks5 socks5.cpp)
target_link_libraries(socks5 PRIVATE fmt::fmt)