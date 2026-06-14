#include "updateLR.h"

void updateLR(double *&A,
              double *&delta,
              int *&nonZeroUserIndexes,
              int *&nonZeroItemIndexes,
              double *&L, double *&R,
              double *&StoreL, double *&StoreR,
              int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
              int &numberOfNonZeroElements,
              double &convergenceCoefficient) {

    int lSize = numberOfUsers * numberOfFeatures;
    int rSize = numberOfFeatures * numberOfItems;

    // Orphaned worksharing loops: they bind to the caller's #pragma omp parallel
    // (in matFact), so the team is forked once for the whole factorization rather
    // than once per iteration. Both snapshot copies are independent, so both carry
    // nowait; the prediction loop's barrier still orders the StoreL/StoreR writes
    // before the update loop reads them.
    #pragma omp for schedule(static) nowait
    for (int idx = 0; idx < lSize; idx++) {
        StoreL[idx] = L[idx];
    }

    #pragma omp for schedule(static) nowait
    for (int idx = 0; idx < rSize; idx++) {
        StoreR[idx] = R[idx];
    }

    #pragma omp for schedule(static)
    for (int l = 0; l < numberOfNonZeroElements; l++) {
        double prediction = 0;
        for (int k = 0; k < numberOfFeatures; k++) {
            prediction += L[nonZeroUserIndexes[l] * numberOfFeatures + k] * R[k * numberOfItems + nonZeroItemIndexes[l]];
        }
        delta[l] = A[nonZeroUserIndexes[l] * numberOfItems + nonZeroItemIndexes[l]] - prediction;
    }

    // Lock-free update: the increments read only the snapshots StoreL/StoreR, never
    // L/R, so each thread accumulates into a private zero-initialised copy of L/R and
    // the array reduction sums those copies into L/R at the end. No atomics.
    #pragma omp for collapse(2) schedule(static) reduction(+ : L[:lSize], R[:rSize])
    for (int l = 0; l < numberOfNonZeroElements; l++) {
        for (int k = 0; k < numberOfFeatures; k++) {
            L[nonZeroUserIndexes[l] * numberOfFeatures + k] += convergenceCoefficient * (2 * delta[l] * StoreR[k * numberOfItems + nonZeroItemIndexes[l]]);
            R[k * numberOfItems + nonZeroItemIndexes[l]] += convergenceCoefficient * (2 * delta[l] * StoreL[nonZeroUserIndexes[l] * numberOfFeatures + k]);
        }
    }
}
