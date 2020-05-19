#include "mpi.h"
//#include "omp.h"

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

    double read_input;
    double initial_lr;
    double update_lr;
    double filter_final_matrix;
    double total_time;

    int processId, numberOfProcesses;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

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

    read_input = MPI_Wtime();
    printf("[readInput][%d] %f\n", processId, read_input - start_time);
    fflush(stdout);

    MPI_Barrier(MPI_COMM_WORLD);

    auto *L = new double[numberOfUsers * numberOfFeatures];
    auto *StoreL = new double[numberOfUsers * numberOfFeatures];
    for (int k = 0; k < numberOfUsers * numberOfFeatures; k++) {
        L[k] = 0;
        StoreL[k] = 0;
    }

    auto *R = new double[numberOfFeatures * numberOfItems];
    auto *StoreR = new double[numberOfFeatures * numberOfItems];
    for (int k = 0; k < numberOfFeatures * numberOfItems; k++) {
        R[k] = 0;
        StoreR[k] = 0;
    }

    initialLR(L, R, numberOfUsers, numberOfItems, numberOfFeatures);

    MPI_Barrier(MPI_COMM_WORLD);
    initial_lr = MPI_Wtime();
    printf("[initialLR][%d] %f\n", processId, initial_lr - read_input);
    fflush(stdout);

    for (int iteration = 0; iteration < numberOfIterations; iteration++) {
        StoreR = R;
        StoreL = L;

        updateLR(A,
                 nonZeroUserIndexes,
                 nonZeroItemIndexes,
                 L, R, StoreL, StoreR,
                 numberOfUsers, numberOfItems, numberOfFeatures,
                 numberOfNonZeroElements,
                 convergenceCoefficient);


    }

    update_lr = MPI_Wtime();
    printf("[updateLR][%d] %f\n", processId, update_lr - initial_lr);
    fflush(stdout);

    auto *B = new double[numberOfUsers * numberOfItems];
    for (int j = 0; j < numberOfUsers * numberOfItems; j++) {
        B[j] = 0;
    }

    auto *BV = new int[numberOfUsers];
    for (int k = 0; k < numberOfUsers; k++) {
        BV[k] = 0;
    }

    filterFinalMatrix(A, B,
                      nonZeroUserIndexes,
                      nonZeroItemIndexes,
                      nonZeroElements,
                      L, R,
                      numberOfUsers, numberOfItems, numberOfFeatures,
                      numberOfNonZeroElements,
                      BV);

    filter_final_matrix = MPI_Wtime();
    printf("[filterFinalMatrix][%d] %f\n", processId, filter_final_matrix - update_lr);
    fflush(stdout);

    std::string outputFileName = inputFileName.substr(0, inputFileName.length() - 2).append("out");
    int numberOfErrors = verifyResult(outputFileName, BV);
    std::cout << "[Errors] " << numberOfErrors << std::endl;

    total_time = MPI_Wtime();
    printf("[Total][%d] %f\n", processId, total_time - start_time);
    fflush(stdout);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    delete[] A;
    delete[] B;
    delete[] BV;

    delete[] StoreL;
    delete[] StoreR;

//    delete[] L;
//    delete[] R;

    return 0;
}