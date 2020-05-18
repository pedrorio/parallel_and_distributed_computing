#include "mpi.h"
#include "omp.h"

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

//    time_t begin = omp_get_wtime();

    double read_input;
    double initial_lr;
    double update_lr;
    double filter_final_matrix;
    double total_time;

//    int processId = 0;
//    int numberOfProcesses = 1;
//    int processId, numberOfProcesses;

    // TODO
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

    std::vector<std::vector<double>> A;
    std::vector<int> nonZeroUserIndexes;
    std::vector<int> nonZeroItemIndexes;
    std::vector<double> nonZeroElements;

    std::string inputFileName = argv[1];

    readInput(inputFileName, A, nonZeroUserIndexes, nonZeroItemIndexes, nonZeroElements,
              numberOfIterations, numberOfFeatures, convergenceCoefficient,
              numberOfUsers, numberOfItems,
              numberOfNonZeroElements, processId, numberOfProcesses);

//     TODO
//    if (processId == ROOT) {
        read_input = MPI_Wtime();
        printf("[readInput][%d] %f\n", processId, read_input - start_time);
        fflush(stdout);
//    }
//    printf("[readInput]\n");
//    fflush(stdout);

    std::vector<std::vector<double>> L(numberOfUsers, std::vector<double>(numberOfFeatures));
    std::vector<std::vector<double>> R(numberOfFeatures, std::vector<double>(numberOfItems));

    initialLR(L, R, numberOfUsers, numberOfItems, numberOfFeatures);

//     TODO
//    if (processId == ROOT) {
        initial_lr = MPI_Wtime();
        printf("[initialLR][%d] %f\n", processId, initial_lr - read_input);
        fflush(stdout);
//    }
//    printf("[initialLR]\n");
//    fflush(stdout);

    std::vector<std::vector<double>> B(numberOfUsers, std::vector<double>(numberOfItems, 0));
    std::vector<std::vector<double>> StoreL;
    std::vector<std::vector<double>> StoreR;
//
////    time_t initial_l_r = omp_get_wtime();
//
    for (int iteration = 0; iteration < numberOfIterations; iteration++) {
        StoreR = R;
        StoreL = L;

        updateLR(A,
                 nonZeroUserIndexes,
                 nonZeroItemIndexes,
                 nonZeroElements,
                 L, R, StoreL, StoreR,
                 numberOfUsers, numberOfItems, numberOfFeatures,
                 numberOfNonZeroElements,
                 convergenceCoefficient);
    }

    // TODO
//    if (processId == ROOT) {
        update_lr = MPI_Wtime();
        printf("[updateLR][%d] %f\n", processId, update_lr - initial_lr);
        fflush(stdout);
//    }
//    printf("[updateLR]\n");
//    fflush(stdout);
    ////    time_t final_filtering = omp_get_wtime();

    std::vector<int> BV(numberOfUsers);
    filterFinalMatrix(A, B,
                      nonZeroUserIndexes,
                      nonZeroItemIndexes,
                      nonZeroElements,
                      L, R,
                      numberOfUsers, numberOfItems, numberOfFeatures,
                      numberOfNonZeroElements,
                      BV);

//    TODO
//    if (processId == ROOT) {
        filter_final_matrix = MPI_Wtime();
        printf("[filterFinalMatrix][%d] %f\n", processId, filter_final_matrix - update_lr);
        fflush(stdout);
//    }
//    printf("[filterFinalMatrix]\n");
//    fflush(stdout);
//    time_t end = omp_get_wtime();
//    std::ofstream logResults("../helpers/comparison.csv", std::ios::app);
//    logResults << inputFileName << ", ";
//    logResults << std::getenv("OMP_NUM_THREADS") << ", ";
//
    // TODO
    if (processId == ROOT) {
        std::string outputFileName = inputFileName.substr(0, inputFileName.length() - 2).append("out");
        int numberOfErrors = verifyResult(outputFileName, BV);
        std::cout << "[Errors] " << numberOfErrors << std::endl;
    }

//    logResults << numberOfErrors << ", ";
//    logResults << numberOfUsers << ", ";
//    logResults << numberOfItems << ", ";
//    logResults << numberOfFeatures << ", ";
//    logResults << numberOfNonZeroElements << ", ";
//    logResults << numberOfIterations << ", ";
//    logResults << double(read_input - begin) << ", ";
//    logResults << double(initial_l_r - read_input) << ", ";
//    logResults << double(final_filtering - initial_l_r) << ", ";
//    logResults << double(end - begin);
//    logResults << std::endl;
//    logResults.close();

// TODO
//    if (processId == ROOT) {
        total_time = MPI_Wtime();
        printf("[Total][%d] %f\n", processId, total_time - start_time);
        fflush(stdout);
//    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

//    printf("[Final]\n");
//    fflush(stdout);

//    time_t end = omp_get_wtime();
//    std::cout << "[Final] " << double(end - begin);
    return 0;
}