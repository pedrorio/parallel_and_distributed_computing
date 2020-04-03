#include "computeB.h"

void computeB(std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
                                          int numberOfUsers, int numberOfItems, int numberOfFeatures,
                                          std::vector<std::vector<double>> &B) {

    for (int i = 0; i < numberOfUsers; i++) {
        for (int j = 0; j < numberOfItems; j++) {
            for (int k = 0; k < numberOfFeatures; k++) {
                B[i][j] += L[i][k] * R[k][j];
            }
        }
    };
}