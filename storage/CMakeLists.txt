cmake_minimum_required(VERSION 3.14)

project(Storage System)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/backend/include)
set(SOURCE_FILES glob "backend/src/*.cpp")

add_subdirectory(ui)