cmake_minimum_required(VERSION 3.14)
project(matFact-mpi CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(MPI REQUIRED)
find_package(OpenMP REQUIRED)

add_executable(matFact-mpi
        matFact.cpp
        src/grid.cpp src/grid.h
        src/config.h
        src/cell.h
        src/readInput.cpp src/readInput.h
        src/distribute.cpp src/distribute.h
        src/initialLR.cpp src/initialLR.h
        src/updateLR.cpp src/updateLR.h
        src/filterFinalMatrix.cpp src/filterFinalMatrix.h)

target_link_libraries(matFact-mpi PRIVATE MPI::MPI_CXX OpenMP::OpenMP_CXX)

enable_testing()
add_subdirectory(tests)
