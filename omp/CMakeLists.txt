cmake_minimum_required(VERSION 3.15)

message(STATUS "set env var")
set(ENV{CC} "/usr/local/opt/llvm/bin/clang")
set(ENV{CXX} "/usr/local/opt/llvm/bin/clang++")
set(ENV{LDFLAGS} "-L/usr/local/opt/llvm/lib")
set(ENV{CPPFLAGS} "-I/usr/local/opt/llvm/include")

project(omp C)
add_executable(matFact-omp matFact-omp.c)

find_package(OpenMP REQUIRED)
target_link_libraries(matFact-omp PRIVATE OpenMP::OpenMP_C)