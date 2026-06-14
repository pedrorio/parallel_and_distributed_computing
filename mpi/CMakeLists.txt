cmake_minimum_required(VERSION 3.14)
project(mpi CXX)
set(CMAKE_CXX_STANDARD 11)

find_package(MPI REQUIRED)

add_executable(
        matFact-mpi
        matFact.cpp
        src/readInput.cpp src/readInput.h
        src/initialLR.cpp src/initialLR.h
        src/updateLR.cpp src/updateLR.h
        src/filterFinalMatrix.cpp src/filterFinalMatrix.h
        src/computeB.cpp src/computeB.h
        src/verifyResult.cpp src/verifyResult.h
)

target_link_libraries(matFact-mpi PRIVATE MPI::MPI_CXX)

enable_testing()
add_subdirectory(tests)
