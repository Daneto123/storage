cmake_minimum_required(VERSION 3.10)

project(StorageApp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

add_executable(StorageApp 
            src/command.cpp,
            src/file.cpp,
            src/function_for_currect_data.cpp,
            src/location.cpp,
            src/product.cpp,
            src/storage.cpp)