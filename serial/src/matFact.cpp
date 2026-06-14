#include <omp.h>
#include "readInput.h"
#include "initialLR.h"
#include "updateLR.h"
#include "filterFinalMatrix.h"
#include "verifyResult.h"
#include "printMatrix.h"

void matFact(std::string inputFileName) {
    double start_time;
    double read_input;
    double initial_lr;
    double total_time;

    start_time = omp_get_wtime();

    auto *A = new double[0];
    auto *nonZeroUserIndexes = new int[0];
    auto *nonZeroItemIndexes = new int[0];
    auto *nonZeroElements = new double[0];

    int numberOfIterations, numberOfFeatures, numberOfUsers, numberOfItems, numberOfNonZeroElements;
    double convergenceCoefficient;


    readInput(inputFileName, A, nonZeroUserIndexes, nonZeroItemIndexes, nonZeroElements,
              numberOfIterations, numberOfFeatures, convergenceCoefficient,
              numberOfUsers, numberOfItems,
              numberOfNonZeroElements);

//    printMatrix("A", A, numberOfUsers, numberOfItems);

    read_input = omp_get_wtime();

    auto *L = new double[(size_t) numberOfUsers * numberOfFeatures];
    auto *R = new double[(size_t) numberOfFeatures * numberOfItems];

    initialLR(L, R, numberOfUsers, numberOfItems, numberOfFeatures);

    initial_lr = omp_get_wtime();

//    printMatrix("L", L, numberOfUsers, numberOfFeatures);
//    printMatrix("R", R, numberOfFeatures, numberOfItems);

    auto *StoreL = new double[(size_t) numberOfUsers * numberOfFeatures];
    auto *StoreR = new double[(size_t) numberOfFeatures * numberOfItems];
    auto *delta = new double[numberOfNonZeroElements];

    for (int iteration = 0; iteration < numberOfIterations; iteration++) {
        updateLR(A,
                 delta,
                 nonZeroUserIndexes,
                 nonZeroItemIndexes,
                 L, R, StoreL, StoreR,
                 numberOfUsers, numberOfItems, numberOfFeatures,
                 numberOfNonZeroElements,
                 convergenceCoefficient);
    }

    double final_filtering = omp_get_wtime();

    delete[] delta;
    delete[] StoreL;
    delete[] StoreR;

    auto *B = new double[(size_t) numberOfUsers * numberOfItems];
    auto *BV = new int[numberOfUsers];

    filterFinalMatrix(A, B, nonZeroUserIndexes,
                      nonZeroItemIndexes,
                      nonZeroElements,
                      L, R,
                      numberOfUsers, numberOfItems, numberOfFeatures,
                      numberOfNonZeroElements,
                      BV);

    delete[] L;
    delete[] R;
    delete[] A;
    delete[] nonZeroUserIndexes;
    delete[] nonZeroItemIndexes;
    delete[] nonZeroElements;
    delete[] B;

    total_time = omp_get_wtime();

    if (std::getenv("LOG_RESULTS")) {
        std::ofstream logResults("../compare/comparison.serial.csv", std::ios::app);
        logResults << inputFileName << ", ";
        logResults << 1 << ", ";
        std::string outputFileName = inputFileName.substr(0, inputFileName.length() - 2).append("out");
        int numberOfErrors = verifyResult(outputFileName, BV);

        std::cout << numberOfErrors << std::endl;

        logResults << numberOfErrors << ", ";
        logResults << numberOfUsers << ", ";
        logResults << numberOfItems << ", ";
        logResults << numberOfFeatures << ", ";
        logResults << numberOfNonZeroElements << ", ";
        logResults << numberOfIterations << ", ";
        logResults << double(read_input - start_time) << ", ";
        logResults << double(initial_lr - read_input) << ", ";
        logResults << double(final_filtering - initial_lr) << ", ";
        logResults << double(total_time - final_filtering) << ", ";
        logResults << double(total_time - start_time);
        logResults << std::endl;
        logResults.close();
    }

    delete[] BV;
}