#include <fstream>
#include <iostream>
#include <filesystem>
#include <omp.h>
#include <vector>
#include <string>

int main(int argc, char *argv[]) {
    std::vector<int> numberOfParallelTasks = {1, 2, 4, 8};
    std::vector<std::string> inputFileNames;

    std::string path("../instances");
    std::string ext(".in");
    for (auto &inputFileName: std::filesystem::recursive_directory_iterator(path)) {
        if (inputFileName.path().extension() == ext) {
            inputFileNames.push_back(inputFileName.path().filename());
        }
    }
    std::cout << "Got the directories" << std::endl;

    std::ofstream logResults("comparison.csv");
    logResults << "fileName, ";
    logResults << "OMP_NUM_THREADS, ";
    logResults << "resultsAreCorrect, ";
    logResults << "numberOfUsers, ";
    logResults << "numberOfItems, ";
    logResults << "numberOfFeatures, ";
    logResults << "numberOfNonZeroElements, ";
    logResults << "numberOfIterations, ";
    logResults << "readInput, ";
    logResults << "initialLR, ";
    logResults << "loop, ";
    logResults << "finalFiltering, ";
    logResults << "total";
    logResults << std::endl;
    logResults.close();

    std::cout << "Wrote the headers" << std::endl;

//    system("cd ../serial && cmake . && make");
//    system("cd ../omp && cmake . && make");
    system("cd ../mpi && mpicxx -std=c++11 matFact-mpi.cpp src/*.cpp -o matFact-mpi");

    for (auto &inputFileName: inputFileNames) {
//        std::cout << "Running " <<  inputFileName << std::endl;
//        system(("../serial/matFact ../instances/" + inputFileName).c_str());
//        std::cout << "Run and logged " <<  inputFileName << std::endl;
        for (auto &numberOfTasks: numberOfParallelTasks) {
//            std::cout << "Running " <<  inputFileName << "for " << numberOfTasks << " parallel tasks with OpenMP" << std::endl;
//            system(("OMP_NUM_THREADS=" + std::to_string(numberOfTasks) +
//                    " ../omp/matFact-omp ../instances/" + inputFileName).c_str());
//            std::cout << "Run and logged " <<  inputFileName << "for " << numberOfTasks << " parallel tasks" << std::endl;
//

            std::cout << "Running " <<  inputFileName << "for " << numberOfTasks << " parallel tasks with MPI" << std::endl;
            system(("PMIX_MCA_gds=hash mpirun -n " + std::to_string(numberOfTasks) +
                    " ../mpi/matFact-mpi ../instances/" + inputFileName).c_str());
            std::cout << "Run and logged " <<  inputFileName << "for " << numberOfTasks << " parallel tasks" << std::endl;
        }
    }

    return 0;
}