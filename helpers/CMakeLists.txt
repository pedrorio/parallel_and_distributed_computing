cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)

project(compare CXX)
add_executable(
        compare
        compare.cpp
        )

find_package(MPI REQUIRED)
find_package(OpenMP REQUIRED)
target_link_libraries(compare PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(compare PRIVATE MPI::MPI_CXX)