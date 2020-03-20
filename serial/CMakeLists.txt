cmake_minimum_required(VERSION 3.15)

message(STATUS "set env var")
set(ENV{CC} "/usr/local/opt/llvm/bin/clang")
set(ENV{CXX} "/usr/local/opt/llvm/bin/clang++")
set(ENV{LDFLAGS} "-L/usr/local/opt/llvm/lib")
set(ENV{CPPFLAGS} "-I/usr/local/opt/llvm/include")

project(serial CXX)
add_executable(
        matFact
        matFact.cpp
        src/readInput.cpp src/readInput.h
        src/writeInitialMatrices.cpp src/writeInitialMatrices.h
        src/writeMatrices.cpp src/writeMatrices.h
        src/initialB.cpp src/initialB.h
        src/initialLR.cpp src/initialLR.h
        src/updateB.cpp src/updateB.h
        src/updateLR.cpp src/updateLR.h
)

find_package(OpenMP REQUIRED)
target_link_libraries(matFact PRIVATE OpenMP::OpenMP_CXX)

add_subdirectory(test)
enable_testing()
