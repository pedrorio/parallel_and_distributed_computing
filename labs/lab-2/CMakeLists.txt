cmake_minimum_required(VERSION 3.15)

message(STATUS "set env var")
set(ENV{CC} "/usr/local/opt/llvm/bin/clang")
set(ENV{CXX} "/usr/local/opt/llvm/bin/clang++")
set(ENV{LDFLAGS} "-L/usr/local/opt/llvm/lib")
set(ENV{CPPFLAGS} "-I/usr/local/opt/llvm/include")

project(lab C)
add_executable(
        omp-2
        omp-2.c
)

find_package(OpenMP REQUIRED)
target_link_libraries(omp-2 PRIVATE OpenMP::OpenMP_C)

#add_subdirectory(test)
#enable_testing()
