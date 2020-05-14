cmake_minimum_required(VERSION 3.14)

project(omp CXX)
set(CMAKE_CXX_STANDARD 11)

add_executable(
        matFact-omp
        matFact-omp.cpp
        src/readInput.cpp src/readInput.h
        src/initialLR.cpp src/initialLR.h
        src/updateLR.cpp src/updateLR.h
        src/filterFinalMatrix.cpp src/filterFinalMatrix.h
        src/computeB.cpp src/computeB.h
        src/verifyResult.cpp src/verifyResult.h
)

find_package(OpenMP REQUIRED)
target_link_libraries(matFact-omp PRIVATE OpenMP::OpenMP_CXX)


#add_subdirectory(test)
#enable_testing()
