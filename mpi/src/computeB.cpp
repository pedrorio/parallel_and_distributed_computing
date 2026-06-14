#include "computeB.h"

void computeB(double *&L, double *&R,
              int &numberOfUsers, int &numberOfItems, int &numberOfFeatures, double *&B) {
    // B is zero-initialised by the caller. The i,k,j order streams R and B
    // sequentially (row-major), unlike an i,j,k order which walks R by columns.
    for (int i = 0; i < numberOfUsers; i++) {
        for (int k = 0; k < numberOfFeatures; k++) {
            double lik = L[i * numberOfFeatures + k];
            for (int j = 0; j < numberOfItems; j++) {
                B[i * numberOfItems + j] += lik * R[k * numberOfItems + j];
            }
        }
    }
}
