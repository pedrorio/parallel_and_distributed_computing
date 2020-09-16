#include "computeB.h"

void computeB(double *&L, double *&R,
              int &numberOfUsers, int &numberOfItems, int &numberOfFeatures, double *&B) {

    for (int i = 0; i < numberOfUsers; i++) {
        for (int j = 0; j < numberOfItems; j++) {
            B[i * numberOfItems + j] = 0;
            for (int k = 0; k < numberOfFeatures; k++) {
                B[i * numberOfItems + j] += L[i * numberOfFeatures + k] * R[k * numberOfItems + j];
            }
        }
    };
}