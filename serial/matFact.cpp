#include <vector>
#include "src/readInput.h"
#include "src/writeInitialMatrices.h"
#include "src/writeMatrices.h"
#include "src/initialB.h"
#include "src/initialLR.h"
#include "src/updateB.h"
#include "src/updateLR.h"

int main(int argc, char *argv[]) {
    int numberOfIterations;
    int numberOfLatentFeatures;
    double convergenceCoefficient;

    int numberOfUsers;
    int numberOfItems;
    int numberOfNonZeroElements;

    std::vector<std::vector<double>> A;
    std::vector<std::vector<double>> L;
    std::vector<std::vector<double>> R;
    std::vector<std::vector<double>> B;

    std::string matrixFileName = "./instances/test.mats";
    std::string inputFileName = argv[1];

    readInput(inputFileName, A,
              numberOfIterations, numberOfLatentFeatures, convergenceCoefficient,
              numberOfUsers, numberOfItems, numberOfNonZeroElements);
    initialB(B, numberOfUsers, numberOfItems);
    initialLR(L, R, numberOfUsers, numberOfItems, numberOfLatentFeatures);
    updateB(B, L, R, numberOfUsers, numberOfItems, numberOfLatentFeatures);


    writeInitialMatrices(matrixFileName, A, L, R, B);

    std::vector<std::vector<double>> StoreL;
    std::vector<std::vector<double>> StoreR;

    for (int iteration = 0; iteration < numberOfIterations; iteration++) {

        StoreL = L;
        StoreR = R;

        updateLR(A, B, L, R, StoreL, StoreR, numberOfUsers, numberOfItems, numberOfLatentFeatures,
                 numberOfNonZeroElements,
                 convergenceCoefficient);
        updateB(B, L, R, numberOfUsers, numberOfItems, numberOfLatentFeatures);
        writeMatrices(matrixFileName, L, R, B, iteration);
    }

    return 0;
}