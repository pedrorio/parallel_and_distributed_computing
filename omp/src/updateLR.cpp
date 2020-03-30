#include "updateLR.h"

void updateLR(std::vector<std::vector<double>> &A, std::vector<std::vector<int>> &nonZeroElementIndexes,
              std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
              std::vector<std::vector<double>> &StoreL, std::vector<std::vector<double>> &StoreR,
              int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
              int &numberOfNonZeroElements,
              double &convergenceCoefficient) {

    std::vector<double> prediction(nonZeroElementIndexes.size(), 0);
    std::vector<double> delta(nonZeroElementIndexes.size(), 0);

    int l, k;

    #pragma omp parallel for collapse(2) private(l, k) shared(numberOfNonZeroElements, numberOfFeatures, nonZeroElementIndexes, prediction, L, R) schedule(static) default(none)
    for (int l = 0; l < numberOfNonZeroElements; l++) {
        for (int k = 0; k < numberOfFeatures; k++) {
            #pragma omp atomic
            prediction[l] += L[nonZeroElementIndexes[l][0]][k] * R[k][nonZeroElementIndexes[l][1]];
        }
    }

    #pragma omp parallel for private(l) shared(numberOfNonZeroElements, nonZeroElementIndexes, delta, prediction, A) schedule(static) default(none)
    for (int l = 0; l < numberOfNonZeroElements; l++) {
        delta[l] = A[nonZeroElementIndexes[l][0]][nonZeroElementIndexes[l][1]] - prediction[l];
    }

    #pragma omp parallel for private(l, k) shared(numberOfNonZeroElements, numberOfFeatures, nonZeroElementIndexes, L, R, StoreL, StoreR, convergenceCoefficient, delta) collapse(2) schedule(static) default(none)
    for (int l = 0; l < numberOfNonZeroElements; l++) {
        for (int k = 0; k < numberOfFeatures; k++) {
            #pragma omp atomic
            L[nonZeroElementIndexes[l][0]][k] += convergenceCoefficient * (2 * delta[l] * StoreR[k][nonZeroElementIndexes[l][1]]);
            #pragma omp atomic
            R[k][nonZeroElementIndexes[l][1]] += convergenceCoefficient * (2 * delta[l] * StoreL[nonZeroElementIndexes[l][0]][k]);
        }
    }
};