#include <vector>
#include <omp.h>
#include "src/readInput.h"
#include "src/initialLR.h"
#include "src/updateLR.h"
#include "src/filterFinalMatrix.h"
#include "src/verifyResult.h"

int main(int argc, char *argv[]) {
    time_t start_time;
    time_t read_input;
    time_t initial_lr;
    time_t update_lr;
    time_t total_time;

    start_time = omp_get_wtime();

    auto *A = new double[0];
    auto *nonZeroUserIndexes = new int[0];
    auto *nonZeroItemIndexes = new int[0];
    auto *nonZeroElements = new double[0];

    int numberOfIterations;
    int numberOfFeatures;
    double convergenceCoefficient;

    int numberOfUsers;
    int numberOfItems;
    int numberOfNonZeroElements;

    int k;

    std::string inputFileName = argv[1];

    readInput(inputFileName, A, nonZeroUserIndexes, nonZeroItemIndexes, nonZeroElements,
              numberOfIterations, numberOfFeatures, convergenceCoefficient,
              numberOfUsers, numberOfItems,
              numberOfNonZeroElements);

    read_input = omp_get_wtime();

    auto *L = new double[numberOfUsers * numberOfFeatures];
    #pragma omp parallel for private(k) shared(L, numberOfUsers, numberOfFeatures) default(none) schedule(static)
    for (int k = 0; k < numberOfUsers * numberOfFeatures; k++) {
        L[k] = 0;
    }

    auto *R = new double[numberOfFeatures * numberOfItems];
    #pragma omp parallel for private(k) shared(R, numberOfFeatures, numberOfItems) default(none) schedule(static)
    for (int k = 0; k < numberOfFeatures * numberOfItems; k++) {
        R[k] = 0;
    }

    initialLR(L, R, numberOfUsers, numberOfItems, numberOfFeatures);

    initial_lr = omp_get_wtime();

    for (int iteration = 0; iteration < numberOfIterations; iteration++) {

        auto *StoreL = new double[numberOfUsers * numberOfFeatures];
        #pragma omp parallel for private(k) shared(StoreL, L, numberOfUsers, numberOfFeatures) default(none) schedule(static)
        for (int k = 0; k < numberOfUsers * numberOfFeatures; k++) {
            StoreL[k] = L[k];
        }

        auto *StoreR = new double[numberOfFeatures * numberOfItems];
        #pragma omp parallel for private(k) shared(StoreR, R, numberOfFeatures, numberOfItems) default(none) schedule(static)
        for (int k = 0; k < numberOfFeatures * numberOfItems; k++) {
            StoreR[k] = R[k];
        }

        updateLR(A, nonZeroUserIndexes,
                 nonZeroItemIndexes,
                 L, R, StoreL, StoreR,
                 numberOfUsers, numberOfItems, numberOfFeatures,
                 numberOfNonZeroElements,
                 convergenceCoefficient);
    }

    time_t final_filtering = omp_get_wtime();

    auto *B = new double[numberOfUsers * numberOfItems];
    #pragma omp parallel for private(k) shared(B, numberOfUsers, numberOfItems) default(none) schedule(static)
    for (int j = 0; j < numberOfUsers * numberOfItems; j++) {
        B[j] = 0;
    }

    auto *BV = new int[numberOfUsers];
    #pragma omp parallel for private(k) shared(BV, numberOfUsers) default(none) schedule(static)
    for (int k = 0; k < numberOfUsers; k++) {
        BV[k] = 0;
    }

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
        std::ofstream logResults("../helpers/comparison.omp.csv", std::ios::app);
        logResults << inputFileName << ", ";
        logResults << std::getenv("OMP_NUM_THREADS") << ", ";
        std::string outputFileName = inputFileName.substr(0, inputFileName.length() - 2).append("out");
        int numberOfErrors = verifyResult(outputFileName, BV);
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

    return 0;
}