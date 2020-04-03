#include <iostream>
#include "filterFinalMatrix.h"
#include "computeB.h"

void filterFinalMatrix(std::vector<std::vector<double>> &A, std::vector<std::vector<double>> &B,
                       std::vector<std::vector<int>> &nonZeroElementIndexes,
                       std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
                       int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
                       int &numberOfNonZeroElements,
                       std::vector<int> &BV) {

    computeB(L, R, numberOfUsers, numberOfItems, numberOfFeatures, B);

    for (int l = 0; l < numberOfNonZeroElements; l++) {
        B[nonZeroElementIndexes[l][0]][nonZeroElementIndexes[l][1]] = 0;
    }

    for (int i = 0; i < numberOfUsers; i++) {
        BV[i] = std::distance(B[i].begin(), std::max_element(B[i].begin(), B[i].end()));
        std::cout << BV[i] << std::endl;
    };
};
