cmake_minimum_required(VERSION 3.14)

project(serial)
set(CMAKE_CXX_STANDARD 11)
include(GNUInstallDirs)

include_directories(src)

add_subdirectory(src)
add_subdirectory(tests)

enable_testing()