#include "updateLR.h"

void updateLR(double *&A, int *&nonZeroUserIndexes,
              int *&nonZeroItemIndexes,
              double *&L, double *&R,
              double *&StoreL, double *&StoreR,
              int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
              int &numberOfNonZeroElements,
              double &convergenceCoefficient) {
    int i, l, k;

    auto *prediction = new double[numberOfNonZeroElements];
    auto *delta = new double[numberOfNonZeroElements];
    #pragma omp parallel for private(i) shared(prediction, delta, numberOfNonZeroElements) default(none) schedule(static)
    for (int i = 0; i < numberOfNonZeroElements; i++) {
        prediction[i] = 0;
        delta[i] = 0;
    }


    #pragma omp parallel shared(numberOfNonZeroElements, numberOfItems, numberOfFeatures, nonZeroUserIndexes, nonZeroItemIndexes, prediction, A, L, R, StoreL, StoreR, convergenceCoefficient, delta) default(none)
    {
        #pragma omp for collapse(2) private(l, k) schedule(guided)
        for (int l = 0; l < numberOfNonZeroElements; l++) {
            for (int k = 0; k < numberOfFeatures; k++) {
                #pragma omp atomic
                prediction[l] +=
                        L[nonZeroUserIndexes[l] * numberOfFeatures + k] * R[k * numberOfItems + nonZeroItemIndexes[l]];
            }
        }

        #pragma omp for private(l) schedule(guided)
        for (int l = 0; l < numberOfNonZeroElements; l++) {
            delta[l] = A[nonZeroUserIndexes[l] * numberOfItems + nonZeroItemIndexes[l]] - prediction[l];
        }

        #pragma omp for private(l, k) collapse(2) schedule(guided)
        for (int l = 0; l < numberOfNonZeroElements; l++) {
            for (int k = 0; k < numberOfFeatures; k++) {
                #pragma omp atomic
                L[nonZeroUserIndexes[l] * numberOfFeatures + k] +=
                        convergenceCoefficient * (2 * delta[l] * StoreR[k * numberOfItems + nonZeroItemIndexes[l]]);
                #pragma omp atomic
                R[k * numberOfItems + nonZeroItemIndexes[l]] +=
                        convergenceCoefficient * (2 * delta[l] * StoreL[nonZeroUserIndexes[l] * numberOfFeatures + k]);
            }
        }
    };

    delete[] StoreL;
    delete[] StoreR;
    delete[] prediction;
    delete[] delta;
};