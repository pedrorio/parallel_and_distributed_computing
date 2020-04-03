#include <fstream>
#include <iostream>
#include <filesystem>
#include <omp.h>
#include <vector>
#include <string>

int main(int argc, char *argv[]) {
    time_t begin = omp_get_wtime();

    std::string inputFileName = argv[1];

    std::vector<int> numberOfParallelTasks = {1, 2, 4, 8};

    std::vector<std::string> inputFileNames;


    std::string path("../instances");
    std::string ext(".in");
    for (auto &inputFileName: std::filesystem::recursive_directory_iterator(path)) {
        if (inputFileName.path().extension() == ext) {
            inputFileNames.push_back(inputFileName.filename());
        }
    }

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

    for (auto &inputFileName: inputFileNames) {
            system("cd ../serial && cmake . && make && ./matFact ../instances/" + inputFileName)
            for (auto& numberOfTasks: numberOfParallelTasks) {
                system("cd ../serial && cmake . && make && OMP_NUM_THREADS=" + numberOfTasks +
                       " ./matFact ../instances/" + inputFileName)
            }
    }

    return 0;
}