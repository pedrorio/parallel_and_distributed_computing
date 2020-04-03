#include "computeB.h"

void computeB(std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
                                          int &numberOfUsers, int &numberOfItems, int &numberOfFeatures, std::vector<std::vector<double>> &B) {

    int i, j, k;

        #pragma omp for collapse(3) private(i, j, k) schedule(guided)
        for (int i = 0; i < numberOfUsers; i++) {
            for (int j = 0; j < numberOfItems; j++) {
                for (int k = 0; k < numberOfFeatures; k++) {
                    #pragma omp atomic
                    B[i][j] += L[i][k] * R[k][j];
                }
            }
        };
}