cmake_minimum_required(VERSION 3.5)

project(animation LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-fcoroutines-ts -stdlib=libc++ -O3)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(SOURCES main.cpp
    horsejump.h
    generator.h
    chessboard.cpp
    chessboard.h
    chessboard.ui
)

qt5_add_resources(SOURCES resource.qrc)
add_executable(animation ${SOURCES})
set_property(TARGET animation PROPERTY CXX_STANDARD 20)
target_link_libraries(animation PRIVATE Qt5::Widgets)

add_executable(console horse_console.cpp horsejump.h generator.h)
set_property(TARGET console PROPERTY CXX_STANDARD 20)
