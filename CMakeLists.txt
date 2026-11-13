cmake_minimum_required(VERSION 3.16)
project(Storage VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

find_package(Qt6 6.2 REQUIRED COMPONENTS Core Gui Widgets)

if(MSVC)
  # Sources contain Bulgarian text in UTF-8.
  add_compile_options(/utf-8 /W4)
else()
  add_compile_options(-Wall -Wextra)
endif()

# Warehouse logic and files: no UI, so it can be tested on its own.
add_library(storage_core STATIC
  src/core/product.cpp
  src/core/inventory.cpp
  src/core/storagefile.cpp
)
target_include_directories(storage_core PUBLIC src/core)
target_link_libraries(storage_core PUBLIC Qt6::Core)

# The window.
add_library(storage_ui STATIC
  src/app/mainwindow.cpp
  src/app/productmodel.cpp
  src/app/productdialog.cpp
  src/app/withdrawdialog.cpp
  src/app/logdialog.cpp
)
target_include_directories(storage_ui PUBLIC src/app)
target_link_libraries(storage_ui PUBLIC storage_core Qt6::Widgets)

add_executable(StorageApp WIN32 MACOSX_BUNDLE src/app/main.cpp)
target_link_libraries(StorageApp PRIVATE storage_ui)
set_target_properties(StorageApp PROPERTIES
  MACOSX_BUNDLE_BUNDLE_NAME "Склад"
  MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
)

include(GNUInstallDirs)
install(TARGETS StorageApp
  BUNDLE DESTINATION .
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

include(CTest)
if(BUILD_TESTING)
  find_package(Qt6 REQUIRED COMPONENTS Test)
  add_executable(storage_tests tests/test_storage.cpp)
  target_link_libraries(storage_tests PRIVATE storage_core Qt6::Test)
  target_compile_definitions(storage_tests PRIVATE STORAGE_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
  add_test(NAME storage_tests COMMAND storage_tests)
endif()
