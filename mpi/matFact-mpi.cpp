#include "mpi.h"
#include <cstdio>
#include <cstdlib>

#include "src/readInput.h"
//#include "src/initialLR.h"
//#include "src/updateLR.h"
//#include "src/filterFinalMatrix.h"
//#include "src/verifyResult.h"



//#include "cstdio"
//#include "string"
//#include "cstdlib"


#define FIRST_ELEMENT(id, p, n) ((id)*(n)/(p))
#define LAST_ELEMENT(id, p, n) (FIRST_ELEMENT((id)+1,p,n)-1)
#define BLOCK_SIZE(id, p, n) (LAST_ELEMENT(id,p,n)-FIRST_ELEMENT(id,p,n)+1)

int main(int argc, char *argv[]) {

    MPI_Status status;
    int processId, numberOfProcesses;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Barrier(MPI_COMM_WORLD);

    double begin = MPI_Wtime();

    int numberOfIterations;
    int numberOfFeatures;
    double convergenceCoefficient;

    int numberOfUsers;
    int numberOfItems;
    int numberOfNonZeroElements;

    std::vector<std::vector<double>> A;
    std::vector<std::vector<int>> nonZeroElementIndexes;

    std::string inputFileName = argv[1];


    readInput(inputFileName, A, nonZeroElementIndexes,
              numberOfIterations, numberOfFeatures, convergenceCoefficient,
              numberOfUsers, numberOfItems,
              numberOfNonZeroElements, processId, numberOfProcesses);

    double read_input = MPI_Wtime();

//        printf("Elapsed time is %f\n", read_input - begin);


    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();




//    std::vector<std::vector<double>> L(numberOfUsers, std::vector<double>(numberOfFeatures));
//    std::vector<std::vector<double>> R(numberOfFeatures, std::vector<double>(numberOfItems));
//
//    initialLR(L, R, numberOfUsers, numberOfItems, numberOfFeatures);
//
//    std::vector<std::vector<double>> B(numberOfUsers, std::vector<double>(numberOfItems,0));
//
//    std::vector<std::vector<double>> StoreL;
//    std::vector<std::vector<double>> StoreR;
//
////    time_t initial_l_r = omp_get_wtime();
//
//    for (int iteration = 0; iteration < numberOfIterations; iteration++) {
//        StoreR = R;
//        StoreL = L;
//
//        updateLR(A, nonZeroElementIndexes,
//                 L, R, StoreL, StoreR,
//                 numberOfUsers, numberOfItems, numberOfFeatures,
//                 numberOfNonZeroElements,
//                 convergenceCoefficient);
//    }
//
////    time_t final_filtering = omp_get_wtime();
//
//    std::vector<int> BV(numberOfUsers);
//    filterFinalMatrix(A, B, nonZeroElementIndexes,
//                      L, R,
//                      numberOfUsers, numberOfItems, numberOfFeatures,
//                      numberOfNonZeroElements,
//                      BV);


//    time_t end = omp_get_wtime();

//    std::ofstream logResults("../helpers/comparison.csv", std::ios::app);
//    logResults << inputFileName << ", ";
//    logResults << std::getenv("OMP_NUM_THREADS") << ", ";
//
//    std::string outputFileName = inputFileName.substr(0, inputFileName.length() - 2).append("out");
//    int numberOfErrors = verifyResult(outputFileName, BV);
//    logResults << numberOfErrors << ", ";
//
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

    return 0;
}