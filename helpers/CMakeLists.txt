cmake_minimum_required(VERSION 3.15)

message(STATUS "set env var")
set(ENV{CC} "/usr/local/opt/llvm/bin/clang")
set(ENV{CXX} "/usr/local/opt/llvm/bin/clang++")
set(ENV{LDFLAGS} "-L/usr/local/opt/llvm/lib")
set(ENV{CPPFLAGS} "-I/usr/local/opt/llvm/include")

set(CMAKE_CXX_STANDARD 17)

project(compare CXX)
add_executable(
        compare
        compare.cpp
        )

find_package(OpenMP REQUIRED)
target_link_libraries(compare PRIVATE OpenMP::OpenMP_CXX)

add_subdirectory(test)
enable_testing()
