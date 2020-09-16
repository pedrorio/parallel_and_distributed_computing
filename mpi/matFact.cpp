#include "mpi.h"

#include "src/readInput.h"
#include "src/initialLR.h"
#include "src/updateLR.h"
#include "src/filterFinalMatrix.h"
#include "src/verifyResult.h"
#include "src/info.h"
#include "src/Array.h"
#include "src/Grid.h"

#define FIRST_ELEMENT(id, p, n) ((id)*(n)/(p))
#define LAST_ELEMENT(id, p, n) (FIRST_ELEMENT((id)+1,p,n)-1)
#define BLOCK_SIZE(id, p, n) (LAST_ELEMENT(id,p,n)-FIRST_ELEMENT(id,p,n)+1)
#define ROOT 0

int main(int argc, char *argv[]) {

    Array *localA = nullptr;

    int *localNonZeroUserIndexes, *localNonZeroItemIndexes = nullptr;
    double *localNonZeroElements = nullptr;

    Array *localL, *localStoreL = nullptr;
    Array *localR, *localStoreR = nullptr;

    double read_input, initial_lr, update_lr, total_time;

    MPI_Init(&argc, &argv);

    CONFIG_INFO configInfo;
    Grid gridInfo;

    double division_time = MPI_Wtime();

    std::string inputFileName = argv[1];


    readInput(inputFileName,
              localA,
              localNonZeroUserIndexes, localNonZeroItemIndexes,
              localNonZeroElements,
              configInfo, gridInfo);

//    MPI_Barrier(MPI_COMM_WORLD);

    read_input = MPI_Wtime();

    L = new Array(configInfo.numberOfUsers, configInfo.numberOfFeatures);
    R = new Array(configInfo.numberOfFeatures, configInfo.numberOfItems);

    initialLR(reinterpret_cast<Array &>(L), reinterpret_cast<Array &>(localL), reinterpret_cast<Array &>(R),
              reinterpret_cast<Array &>(localR), configInfo);

    initial_lr = MPI_Wtime();

    std::cout << "after initialLR process " << processId << std::endl;
    fflush(stdout);

    for (int iteration = 0; iteration < configInfo.numberOfIterations; iteration++) {

        StoreL = new Array(configInfo.numberOfUsers, configInfo.numberOfFeatures);
        for (int k = 0; k < configInfo.numberOfUsers; k++) {
            for (int i = 0; i < configInfo.numberOfFeatures; i++) {
                &StoreL(k, i) = L(k, i);
            }
        }

        StoreR = new Array(configInfo.numberOfFeatures, configInfo.numberOfItems);
        for (int k = 0; k < configInfo.numberOfFeatures; k++) {
            for (int i = 0; i < configInfo.numberOfItems; i++) {
                StoreR(k, i) = R(k, i);
            }
        }

        updateLR(localA,
                 localNonZeroUserIndexes,
                 localNonZeroItemIndexes,
                 StoreL, StoreR,
                 configInfo, gridInfo);

        delete[] StoreL;
        delete[] StoreR;
    }
//
//    update_lr = MPI_Wtime();
//
//    auto *B = new double[numberOfUsers * numberOfItems];
//    for (int j = 0; j < numberOfUsers * numberOfItems; j++) {
//        B[j] = 0;
//    }
//
//    auto *BV = new int[numberOfUsers];
//    for (int k = 0; k < numberOfUsers; k++) {
//        BV[k] = 0;
//    }
//
//    if (processId == ROOT) {
//        filterFinalMatrix(A, B,
//                          nonZeroUserIndexes,
//                          nonZeroItemIndexes,
//                          nonZeroElements,
//                          L, R,
//                          numberOfUsers, numberOfItems, numberOfFeatures,
//                          numberOfNonZeroElements,
//                          BV);
//    }

    total_time = MPI_Wtime();

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    if (std::getenv("LOG_RESULTS")) {
        std::ofstream logResults("../helpers/comparison.mpi.csv", std::ios::app);

        logResults << inputFileName << ", ";
        logResults << numberOfProcesses << ", ";

        std::string outputFileName = inputFileName.substr(0, inputFileName.length() - 2).append("out");
//        int numberOfErrors = verifyResult(outputFileName, BV);
//        logResults << numberOfErrors << ", ";

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

//        std::cout << double(total_time - start_time) << std::endl;
    }
//    std::string outputFileName = inputFileName.substr(0, inputFileName.length() - 2).append("out");
//    int numberOfErrors = verifyResult(outputFileName, BV);
//
//    std::cout << numberOfErrors << std::endl;

    delete[] A;
//    delete[] B;
//    delete[] BV;
//
//    delete[] L;
//    delete[] R;

    return 0;
}