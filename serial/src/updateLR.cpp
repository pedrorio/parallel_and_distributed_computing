#include "updateLR.h"

#include <algorithm>

void updateLR(double *&A,
              double *&delta,
              int *&nonZeroUserIndexes,
              int *&nonZeroItemIndexes,
              double *&L, double *&R,
              double *&StoreL, double *&StoreR,
              int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
              int &numberOfNonZeroElements,
              double &convergenceCoefficient) {

    // Snapshot L and R; every read below uses these pre-iteration values.
    std::copy(L, L + (size_t) numberOfUsers * numberOfFeatures, StoreL);
    std::copy(R, R + (size_t) numberOfFeatures * numberOfItems, StoreR);

    for (int l = 0; l < numberOfNonZeroElements; l++) {
        double prediction = 0;
        for (int k = 0; k < numberOfFeatures; k++) {
            prediction += L[nonZeroUserIndexes[l] * numberOfFeatures + k] * R[k * numberOfItems + nonZeroItemIndexes[l]];
        }
        delta[l] = A[nonZeroUserIndexes[l] * numberOfItems + nonZeroItemIndexes[l]] - prediction;
    }

    for (int l = 0; l < numberOfNonZeroElements; l++) {
        for (int k = 0; k < numberOfFeatures; k++) {
            L[nonZeroUserIndexes[l] * numberOfFeatures + k] += convergenceCoefficient * (2 * delta[l] * StoreR[k * numberOfItems + nonZeroItemIndexes[l]]);
            R[k * numberOfItems + nonZeroItemIndexes[l]] += convergenceCoefficient * (2 * delta[l] * StoreL[nonZeroUserIndexes[l] * numberOfFeatures + k]);
        }
    }
}
