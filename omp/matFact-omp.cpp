#include <vector>
#include "src/readInput.h"
#include "src/writeInitialMatrices.h"
#include "src/writeMatrices.h"
#include "src/initialLR.h"
#include "src/updateLR.h"
#include "src/filterFinalMatrix.h"
#include <omp.h>

int main(int argc, char *argv[]) {
    time_t begin = clock();

//    std::cout << omp_get_max_threads() << std::endl;

    int numberOfIterations;
    int numberOfFeatures;
    double convergenceCoefficient;

    int numberOfUsers;
    int numberOfItems;
    int numberOfNonZeroElements;

    std::vector<std::vector<double>> A;
    std::vector<std::vector<double>> L;
    std::vector<std::vector<double>> R;
    std::vector<std::vector<int>> nonZeroElementIndexes;

//    std::string matrixFileName = "./instances/test.mats";
    std::string inputFileName = argv[1];

    readInput(inputFileName, A, nonZeroElementIndexes,
              numberOfIterations, numberOfFeatures, convergenceCoefficient,
              numberOfUsers, numberOfItems, numberOfNonZeroElements);

    initialLR(L, R, numberOfUsers, numberOfItems, numberOfFeatures);

//    writeInitialMatrices(matrixFileName, A, L, R, numberOfUsers, numberOfItems, numberOfFeatures);

    std::vector<std::vector<double>> StoreL;
    std::vector<std::vector<double>> StoreR;

    for (int iteration = 0; iteration < numberOfIterations; iteration++) {
        StoreL = L;
        StoreR = R;

        updateLR(A, nonZeroElementIndexes,
                 L, R, StoreL, StoreR,
                 numberOfUsers, numberOfItems, numberOfFeatures,
                 numberOfNonZeroElements,
                 convergenceCoefficient);

//        writeMatrices(matrixFileName, L, R, iteration, numberOfUsers, numberOfItems, numberOfFeatures);
    }

    filterFinalMatrix(A, nonZeroElementIndexes,
                      L, R,
                      numberOfUsers, numberOfItems, numberOfFeatures);


    time_t end = clock();
    printf("End: %.3f\n", double((end - begin)) / CLOCKS_PER_SEC);

    return 0;
}