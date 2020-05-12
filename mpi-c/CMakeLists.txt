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

project(mpi C)
set(CMAKE_C_STANDARD 99)

add_executable(
        matFact-mpi
        matFact-mpi.c
        src/blockAllocation.h
        src/readInput.h
        src/initialLR.h
        src/updateLR.h
        src/filterFinalMatrix.h
        src/computeB.h
        src/verifyResult.h
        src/structures.h)

find_package(MPI REQUIRED)
target_link_libraries(matFact-mpi PRIVATE MPI::MPI_C)