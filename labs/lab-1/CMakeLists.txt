cmake_minimum_required(VERSION 3.15)

message(STATUS "set env var")
set(ENV{CC} "/usr/local/opt/llvm/bin/clang")
set(ENV{CXX} "/usr/local/opt/llvm/bin/clang++")
set(ENV{LDFLAGS} "-L/usr/local/opt/llvm/lib")
set(ENV{CPPFLAGS} "-I/usr/local/opt/llvm/include")

project(lab C)
add_executable(
        omp-1
        omp-1.c
)

find_package(OpenMP REQUIRED)
target_link_libraries(omp-1 PRIVATE OpenMP::OpenMP_C)

#add_subdirectory(test)
#enable_testing()
