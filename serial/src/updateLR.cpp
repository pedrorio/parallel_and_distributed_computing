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

    int i, l, k;

    for (int i = 0; i < numberOfFeatures; i++) {
        for (int k = 0; k < numberOfUsers; k++) {
            StoreL[k * numberOfFeatures + i] = L[k * numberOfFeatures + i];
        }
        for (int k = 0; k < numberOfItems; k++) {
            StoreR[i * numberOfItems + k] = R[i * numberOfItems + k];
        }
    }

    for (int l = 0; l < numberOfNonZeroElements; l++) {
        prediction[l] = 0;
        delta[l] = 0;
        for (int k = 0; k < numberOfFeatures; k++) {
            prediction[l] += L[nonZeroUserIndexes[l] * numberOfFeatures + k] * R[k * numberOfItems + nonZeroItemIndexes[l]];
        }
        delta[l] = A[nonZeroUserIndexes[l] * numberOfItems + nonZeroItemIndexes[l]] - prediction[l];
    }

    for (int l = 0; l < numberOfNonZeroElements; l++) {
        for (int k = 0; k < numberOfFeatures; k++) {
            L[nonZeroUserIndexes[l] * numberOfFeatures + k] += convergenceCoefficient * (2 * delta[l] * StoreR[k * numberOfItems + nonZeroItemIndexes[l]]);
            R[k * numberOfItems + nonZeroItemIndexes[l]] += convergenceCoefficient * (2 * delta[l] * StoreL[nonZeroUserIndexes[l] * numberOfFeatures + k]);
        }
    }
};