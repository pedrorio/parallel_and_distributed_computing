cmake_minimum_required(VERSION 3.14)

if(UNIX AND NOT APPLE)
else()
    message(STATUS "set env var")
    set(ENV{CC} "/usr/local/opt/llvm/bin/clang")
    set(ENV{CXX} "/usr/local/opt/llvm/bin/clang++")
    set(ENV{LDFLAGS} "-L/usr/local/opt/llvm/lib")
    set(ENV{CPPFLAGS} "-I/usr/local/opt/llvm/include")
endif()

project(mpi CXX)
set(CMAKE_CXX_STANDARD 11)

add_executable(matFact-mpi matFact-mpi.cpp)

find_package(OpenMP REQUIRED)
target_link_libraries(matFact-mpi PRIVATE OpenMP::OpenMP_CXX)