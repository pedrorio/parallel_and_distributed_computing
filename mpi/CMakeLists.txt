cmake_minimum_required(VERSION 3.14)
project(mpi CXX)
set(CMAKE_CXX_STANDARD 11)

add_executable(
        matFact-mpi
        matFact-mpi.cpp
        src/readInput.cpp src/readInput.h
        src/initialLR.cpp src/initialLR.h
        src/updateLR.cpp src/updateLR.h
        src/filterFinalMatrix.cpp src/filterFinalMatrix.h
        src/computeB.cpp src/computeB.h
        src/verifyResult.cpp src/verifyResult.h
)

find_package(MPI REQUIRED)
find_package(OpenMP REQUIRED)
target_link_libraries(matFact-mpi PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(matFact-mpi PRIVATE MPI::MPI_CXX)