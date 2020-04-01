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
    int i, j;

    std::vector<std::vector<double>> A;
    std::vector<std::vector<int>> nonZeroElementIndexes;

    // maybe out?
    std::vector<int> nonZeroUserIndexes;
    std::vector<int> nonZeroItemIndexes;

    std::string inputFileName = argv[1];

    readInput(inputFileName, A, nonZeroElementIndexes,
              numberOfIterations, numberOfFeatures, convergenceCoefficient,
              numberOfUsers, numberOfItems,
              numberOfNonZeroElements, nonZeroUserIndexes, nonZeroItemIndexes);

    time_t read_input = omp_get_wtime();
    printf("read_input: %.6f\n", double(read_input - begin));

    std::vector<std::vector<double>> L(numberOfUsers, std::vector<double>(numberOfFeatures));
    std::vector<std::vector<double>> R(numberOfFeatures, std::vector<double>(numberOfItems));

    initialLR(L, R, numberOfUsers, numberOfItems, numberOfFeatures);

    std::vector<std::vector<double>> StoreL;
    std::vector<std::vector<double>> StoreR;

    time_t initial_l_r = omp_get_wtime();
    printf("initial_l_r: %.6f\n", double(initial_l_r - read_input));

    for (int iteration = 0; iteration < numberOfIterations; iteration++) {
       StoreL = L;
       StoreR = R;

//        std::cout << "iteration: " << iteration << std::endl;
        updateLR(A, nonZeroElementIndexes,
                 L, R, StoreL, StoreR,
                 numberOfUsers, numberOfItems, numberOfFeatures,
                 numberOfNonZeroElements,
                 convergenceCoefficient);
    }

    time_t final_filtering = omp_get_wtime();

    std::vector<int> BV(numberOfUsers);
    filterFinalMatrix(A, nonZeroElementIndexes,
                      L, R,
                      numberOfUsers, numberOfItems, numberOfFeatures,
                      numberOfNonZeroElements,
                      BV);


    time_t end = omp_get_wtime();
    printf("final_filtering: %.6f\n", double(end - final_filtering));


    std::string outputFileName = inputFileName.substr(0, inputFileName.length() - 2).append("out");
    verifyResult(outputFileName, BV);

    printf("total: %.6f\n", double(end - begin));

    return 0;
}