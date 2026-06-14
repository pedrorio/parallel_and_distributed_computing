#include "computeB.h"

void computeB(double *&L, double *&R, int &numberOfUsers, int &numberOfItems, int &numberOfFeatures, double *&B) {

    // Each thread owns a disjoint set of rows i, so no two threads write the same
    // B[i*items+j]; the i,k,j order also streams R and B sequentially. No atomic
    // is needed.
    #pragma omp for schedule(static)
    for (int i = 0; i < numberOfUsers; i++) {
        for (int j = 0; j < numberOfItems; j++) {
            B[i * numberOfItems + j] = 0;
        }
        for (int k = 0; k < numberOfFeatures; k++) {
            double lik = L[i * numberOfFeatures + k];
            for (int j = 0; j < numberOfItems; j++) {
                B[i * numberOfItems + j] += lik * R[k * numberOfItems + j];
            }
        }
    }
}
