#include "computeB.h"

void computeB(double *&L, double *&R,
              int &numberOfUsers, int &numberOfItems, int &numberOfFeatures, double *&B) {

    int i, j, k;

    #pragma omp for collapse(3) private(i, j, k) schedule(guided)
    for (int i = 0; i < numberOfUsers; i++) {
        for (int j = 0; j < numberOfItems; j++) {
            for (int k = 0; k < numberOfFeatures; k++) {
                #pragma omp atomic
                B[i * numberOfItems + j] += L[i * numberOfFeatures + k] * R[k * numberOfItems + j];
            }
        }
    };
}