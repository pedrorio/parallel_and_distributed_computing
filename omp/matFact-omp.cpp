#include <vector>
#include <omp.h>
#include "src/readInput.h"
#include "src/writeInitialMatrices.h"
#include "src/writeMatrices.h"
#include "src/initialLR.h"
#include "src/updateLR.h"
#include "src/filterFinalMatrix.h"
#include "src/verifyResult.h"

int main(int argc, char *argv[]) {
    time_t begin = omp_get_wtime();


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

    std::vector<int> nonZeroUserIndexes;
    std::vector<int> nonZeroItemIndexes;

    std::vector<std::vector<double>> StoreL;
    std::vector<std::vector<double>> StoreR;


    std::string inputFileName = argv[1];

    readInput(inputFileName, A, nonZeroElementIndexes,
              numberOfIterations, numberOfFeatures, convergenceCoefficient,
              numberOfUsers, numberOfItems, numberOfNonZeroElements, nonZeroUserIndexes, nonZeroItemIndexes);

    initialLR(L, R, numberOfUsers, numberOfItems, numberOfFeatures);


//#pragma omp parallel for private(StoreL, StoreR) default(none)
#pragma omp parallel for private(StoreL, StoreR)
    for (int iteration = 0; iteration < numberOfIterations; iteration++) {
#pragma omp critical
        StoreL = L;
#pragma omp critical
        StoreR = R;
//    };

        if (iteration % 10 == 0) {
            std::cout << "iteration: " << iteration << std::endl;
        }
        updateLR(A, nonZeroElementIndexes,
                 L, R, StoreL, StoreR,
                 numberOfUsers, numberOfItems, numberOfFeatures,
                 numberOfNonZeroElements,
                 convergenceCoefficient);
    }


    std::vector<int> BV;
    filterFinalMatrix(A, nonZeroElementIndexes,
                      L, R,
                      numberOfUsers, numberOfItems, numberOfFeatures, BV);


    time_t end = omp_get_wtime();
    printf("End: %.3f\n", double(end - begin));

    std::string outputFileName = inputFileName.substr(0, inputFileName.length() - 2).append("out");
    verifyResult(outputFileName, BV);

    return 0;
}