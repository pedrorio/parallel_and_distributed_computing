#include "updateLR.h"

void updateLR(double *&A,
              double *&prediction, double *&delta,
              int *&nonZeroUserIndexes,
              int *&nonZeroItemIndexes,
              double *&L, double *&R,
              double *&StoreL, double *&StoreR,
              int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
              int &numberOfNonZeroElements,
              double &convergenceCoefficient) {
    int l, k;

    #pragma omp parallel shared(numberOfNonZeroElements, numberOfUsers, numberOfItems, numberOfFeatures, nonZeroUserIndexes, nonZeroItemIndexes, prediction, A, L, R, StoreL, StoreR, convergenceCoefficient, delta) default(none)
    {
        // Snapshot L and R as plain contiguous (stride-1) copies. The two copies
        // are independent, so the first carries `nowait` to avoid an extra barrier.
        #pragma omp for schedule(static) nowait
        for (int idx = 0; idx < numberOfUsers * numberOfFeatures; idx++) {
            StoreL[idx] = L[idx];
        }

        #pragma omp for schedule(static)
        for (int idx = 0; idx < numberOfFeatures * numberOfItems; idx++) {
            StoreR[idx] = R[idx];
        }

        #pragma omp for private(l, k) schedule(static)
        for (int l = 0; l < numberOfNonZeroElements; l++) {
            prediction[l] = 0;
            delta[l] = 0;
            for (int k = 0; k < numberOfFeatures; k++) {
                prediction[l] += L[nonZeroUserIndexes[l] * numberOfFeatures + k] * R[k * numberOfItems + nonZeroItemIndexes[l]];
            }
            delta[l] = A[nonZeroUserIndexes[l] * numberOfItems + nonZeroItemIndexes[l]] - prediction[l];
        }

        #pragma omp for private(l, k) collapse(2) schedule(static)
        for (int l = 0; l < numberOfNonZeroElements; l++) {
            for (int k = 0; k < numberOfFeatures; k++) {
                #pragma omp atomic
                L[nonZeroUserIndexes[l] * numberOfFeatures + k] += convergenceCoefficient * (2 * delta[l] * StoreR[k * numberOfItems + nonZeroItemIndexes[l]]);
                #pragma omp atomic
                R[k * numberOfItems + nonZeroItemIndexes[l]] += convergenceCoefficient * (2 * delta[l] * StoreL[nonZeroUserIndexes[l] * numberOfFeatures + k]);
            }
        }
    }
}