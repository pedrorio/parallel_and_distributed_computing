cmake_minimum_required(VERSION 3.14)
project(matFact-grid CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(MPI REQUIRED)
find_package(OpenMP REQUIRED)

add_executable(matFact-grid
        matFact.cpp
        grid.cpp
        readInput.cpp
        distribute.cpp
        initialLR.cpp
        updateLR.cpp
        filterFinalMatrix.cpp)

target_link_libraries(matFact-grid PRIVATE MPI::MPI_CXX OpenMP::OpenMP_CXX)
