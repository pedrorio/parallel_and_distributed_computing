//
// Created by Pedro Rio on 26/03/2020.
//

#include "computeB.h"

std::vector<std::vector<double>> computeB(std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
                                          int numberOfUsers, int numberOfItems, int numberOfFeatures) {

    std::vector<std::vector<double>> B(numberOfUsers, std::vector<double>(numberOfItems));

    int i, j, k;

    #pragma omp parallel shared(numberOfUsers, numberOfItems, numberOfFeatures, B, L, R) default(none)
    {
        #pragma omp for collapse(3) private(i, j, k)
        for (int i = 0; i < numberOfUsers; i++) {
            for (int j = 0; j < numberOfItems; j++) {
                for (int k = 0; k < numberOfFeatures; k++) {
                    #pragma omp atomic
                    B[i][j] += L[i][k] * R[k][j];
                }
            }
        };
    }

    return B;
}