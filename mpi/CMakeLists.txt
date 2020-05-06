cmake_minimum_required(VERSION 3.14)

#if (UNIX AND NOT APPLE)
#else ()
#    message(STATUS "set env var")
#    #    set(ENV{CC} "gcc-9")
#    #    set(ENV{CXX} "g++-9")
#    #    set(OMPI_CXX g++-9)
#    #    set(OMPI_CC gcc-9)
#
#    #    set(ENV{CC} "clang")
#    #    set(ENV{CXX} "clang++")
#    #    set(ENV{OMPI_CXX} "clang++")
#    #    set(ENV{OMPI_CC} "clang")
#
#    # see https://github.com/open-mpi/ompi/issues/7516
##    set(ENV{PMIX_MCA_gds} "hash")
#endif ()

project(mpi CXX)
set(CMAKE_CXX_STANDARD 11)

add_executable(
        matFact-mpi
        matFact-mpi.cpp
        src/readInput.cpp src/readInput.h
        #        src/initialLR.cpp src/initialLR.h
        #        src/updateLR.cpp src/updateLR.h
        #        src/filterFinalMatrix.cpp src/filterFinalMatrix.h
        #        src/computeB.cpp src/computeB.h
        #        src/verifyResult.cpp src/verifyResult.h
)

find_package(MPI REQUIRED)
target_link_libraries(matFact-mpi PRIVATE MPI::MPI_CXX)