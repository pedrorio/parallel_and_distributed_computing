#include "mpi.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "src/readInput.h"
#include "src/initialLR.h"
#include "src/updateLR.h"
#include "src/filterFinalMatrix.h"
#include "src/verifyResult.h"

#define ROOT 0

int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);

    int processId, numberOfProcesses;
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);

    if (argc < 2) {
        if (processId == ROOT) {
            std::cerr << "usage: matFact-mpi <instance.in>" << std::endl;
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    std::string inputFileName = argv[1];

    double start_time = MPI_Wtime();

    // Empty placeholders so readInput can delete[] them before allocating.
    double *A = new double[0];
    int *nonZeroUserIndexes = new int[0];
    int *nonZeroItemIndexes = new int[0];
    double *nonZeroElements = new double[0];

    int numberOfIterations, numberOfFeatures, numberOfUsers, numberOfItems, numberOfNonZeroElements;
    double convergenceCoefficient;

    readInput(inputFileName, A, nonZeroUserIndexes, nonZeroItemIndexes, nonZeroElements,
              numberOfIterations, numberOfFeatures, convergenceCoefficient,
              numberOfUsers, numberOfItems, numberOfNonZeroElements,
              processId, numberOfProcesses);

    double read_input = MPI_Wtime();

    // initialLR is deterministic (srandom(1)), so L and R start out identical on
    // every process without any communication.
    double *L = new double[numberOfUsers * numberOfFeatures];
    double *R = new double[numberOfFeatures * numberOfItems];

    initialLR(L, R, numberOfUsers, numberOfItems, numberOfFeatures);

    double initial_lr = MPI_Wtime();

    double *StoreL = new double[numberOfUsers * numberOfFeatures];
    double *StoreR = new double[numberOfFeatures * numberOfItems];
    double *dL = new double[numberOfUsers * numberOfFeatures];
    double *dR = new double[numberOfFeatures * numberOfItems];

    for (int iteration = 0; iteration < numberOfIterations; iteration++) {
        updateLR(A, nonZeroUserIndexes, nonZeroItemIndexes,
                 L, R, StoreL, StoreR, dL, dR,
                 numberOfUsers, numberOfItems, numberOfFeatures,
                 numberOfNonZeroElements, convergenceCoefficient,
                 processId, numberOfProcesses);
    }

    double update_lr = MPI_Wtime();

    delete[] StoreL;
    delete[] StoreR;
    delete[] dL;
    delete[] dR;

    // L and R are identical on every process; the root builds the final
    // recommendation matrix and prints one recommended item per user.
    int *BV = new int[numberOfUsers];
    double *B = nullptr;
    if (processId == ROOT) {
        B = new double[numberOfUsers * numberOfItems];
        for (int j = 0; j < numberOfUsers * numberOfItems; j++) {
            B[j] = 0.0;
        }

        filterFinalMatrix(A, B, nonZeroUserIndexes, nonZeroItemIndexes, nonZeroElements,
                          L, R, numberOfUsers, numberOfItems, numberOfFeatures,
                          numberOfNonZeroElements, BV);
    }

    double total_time = MPI_Wtime();

    if (processId == ROOT && std::getenv("LOG_RESULTS")) {
        std::ofstream logResults("../compare/comparison.mpi.csv", std::ios::app);
        logResults << inputFileName << ", ";
        logResults << numberOfProcesses << ", ";
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
        logResults << double(update_lr - initial_lr) << ", ";
        logResults << double(total_time - update_lr) << ", ";
        logResults << double(total_time - start_time);
        logResults << std::endl;
        logResults.close();
    }

    delete[] A;
    delete[] nonZeroUserIndexes;
    delete[] nonZeroItemIndexes;
    delete[] nonZeroElements;
    delete[] L;
    delete[] R;
    delete[] BV;
    delete[] B;

    MPI_Finalize();
    return 0;
}
