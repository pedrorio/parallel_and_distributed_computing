#include "mpi.h"

#include "src/readInput.h"
#include "src/initialLR.h"
#include "src/updateLR.h"
#include "src/filterFinalMatrix.h"
#include "src/verifyResult.h"

#define FIRST_ELEMENT(id, p, n) ((id)*(n)/(p))
#define LAST_ELEMENT(id, p, n) (FIRST_ELEMENT((id)+1,p,n)-1)
#define BLOCK_SIZE(id, p, n) (LAST_ELEMENT(id,p,n)-FIRST_ELEMENT(id,p,n)+1)
#define ROOT 0

int main(int argc, char *argv[]) {

    auto *A = new double[0];
    auto *nonZeroUserIndexes = new int[0];
    auto *nonZeroItemIndexes = new int[0];
    auto *nonZeroElements = new double[0];

    int processId, numberOfProcesses;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    int numberOfIterations;
    int numberOfFeatures;
    double convergenceCoefficient;

    int numberOfUsers;
    int numberOfItems;
    int numberOfNonZeroElements;

    std::string inputFileName = argv[1];

    readInput(inputFileName, A, nonZeroUserIndexes, nonZeroItemIndexes, nonZeroElements,
              numberOfIterations, numberOfFeatures, convergenceCoefficient,
              numberOfUsers, numberOfItems,
              numberOfNonZeroElements, processId, numberOfProcesses);

    auto *L = new double[numberOfUsers * numberOfFeatures];
    for (int k = 0; k < numberOfUsers * numberOfFeatures; k++) {
        L[k] = 0;
    }

    auto *R = new double[numberOfFeatures * numberOfItems];
    for (int k = 0; k < numberOfFeatures * numberOfItems; k++) {
        R[k] = 0;
    }

    initialLR(L, R, numberOfUsers, numberOfItems, numberOfFeatures);

    for (int iteration = 0; iteration < numberOfIterations; iteration++) {

        auto *StoreL = new double[numberOfUsers * numberOfFeatures];
        for (int k = 0; k < numberOfUsers * numberOfFeatures; k++) {
            StoreL[k] = L[k];
        }

        auto *StoreR = new double[numberOfFeatures * numberOfItems];
        for (int k = 0; k < numberOfFeatures * numberOfItems; k++) {
            StoreR[k] = R[k];
        }

        updateLR(A,
                 nonZeroUserIndexes,
                 nonZeroItemIndexes,
                 L, R, StoreL, StoreR,
                 numberOfUsers, numberOfItems, numberOfFeatures,
                 numberOfNonZeroElements,
                 convergenceCoefficient,
                 processId, numberOfProcesses);

        delete[] StoreL;
        delete[] StoreR;
    }

    auto *B = new double[numberOfUsers * numberOfItems];
    for (int j = 0; j < numberOfUsers * numberOfItems; j++) {
        B[j] = 0;
    }

    auto *BV = new int[numberOfUsers];
    for (int k = 0; k < numberOfUsers; k++) {
        BV[k] = 0;
    }

    if (processId == ROOT) {
        filterFinalMatrix(A, B,
                          nonZeroUserIndexes,
                          nonZeroItemIndexes,
                          nonZeroElements,
                          L, R,
                          numberOfUsers, numberOfItems, numberOfFeatures,
                          numberOfNonZeroElements,
                          BV);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    delete[] A;
    delete[] B;
    delete[] BV;

    delete[] L;
    delete[] R;

    return 0;
}