#include <iostream>
#include "filterFinalMatrix.h"
#include "computeB.h"

void filterFinalMatrix(std::vector<std::vector<double>> &A, std::vector<std::vector<double>> &B, std::vector<std::vector<int>> &nonZeroElementIndexes,
                       std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
                       int &numberOfUsers, int &numberOfItems, int &numberOfFeatures, int &numberOfNonZeroElements,
                       std::vector<int> &BV) {

    int i, j, l;
#pragma omp parallel shared(numberOfUsers, numberOfItems, numberOfFeatures, A, B, L, R, numberOfNonZeroElements, nonZeroElementIndexes, BV)  default(none)
    {
        computeB(L, R, numberOfUsers, numberOfItems, numberOfFeatures, B);

#pragma omp for private(l) schedule(static)
        for (int l = 0; l < numberOfNonZeroElements; l++) {
            B[nonZeroElementIndexes[l][0]][nonZeroElementIndexes[l][1]] = 0;
        }

#pragma omp for private(i) schedule(static)
        for (int i = 0; i < numberOfUsers; i++) {
            int maxElementIndex = std::distance(B[i].begin(), std::max_element(B[i].begin(), B[i].end()));
            BV[i] = maxElementIndex;
        };
    };

    for (int i = 0; i < numberOfUsers; i++) {
        std::cout << BV[i] << std::endl;
    };

};
