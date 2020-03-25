#include <vector>
#include "src/readInput.h"
#include "src/writeInitialMatrices.h"
#include "src/writeMatrices.h"
#include "src/initialB.h"
#include "src/initialLR.h"
#include "src/updateB.h"
#include "src/updateLR.h"

int main(int argc, char *argv[]) {
    time_t begin = clock();

    int numberOfIterations;
    int numberOfFeatures;
    double convergenceCoefficient;

    int numberOfUsers;
    int numberOfItems;
    int numberOfNonZeroElements;

    std::vector<std::vector<double>> A;
    std::vector<std::vector<double>> L;
    std::vector<std::vector<double>> R;
    std::vector<std::vector<double>> B;
    std::vector<std::vector<int>> nonZeroElementIndexes;

    //  Likj - ikj sums all j
    //  Rkji - kji sums all i

    // get an array of indexes representing non zero elements


    std::string matrixFileName = "./instances/test.mats";
    std::string inputFileName = argv[1];

    readInput(inputFileName, A, nonZeroElementIndexes,
              numberOfIterations, numberOfFeatures, convergenceCoefficient,
              numberOfUsers, numberOfItems, numberOfNonZeroElements);

    initialB(B, numberOfUsers, numberOfItems);
    initialLR(L, R, numberOfUsers, numberOfItems, numberOfFeatures);
    updateB(B, L, R, numberOfUsers, numberOfItems, numberOfFeatures);

    writeInitialMatrices(matrixFileName, A, L, R, B);

    std::vector<std::vector<double>> StoreL;
    std::vector<std::vector<double>> StoreR;

    for (int iteration = 0; iteration < numberOfIterations; iteration++) {
        StoreL = L;
        StoreR = R;

        updateLR(A, nonZeroElementIndexes, B, L, R, StoreL, StoreR, numberOfUsers, numberOfItems,
                 numberOfFeatures,
                 numberOfNonZeroElements,
                 convergenceCoefficient);
        writeMatrices(matrixFileName, L, R, iteration);

    }

    time_t end = clock();
    printf("End: %.3f\n", double((end - begin)) / CLOCKS_PER_SEC);

    return 0;
}