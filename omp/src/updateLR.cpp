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
    #pragma omp parallel shared(numberOfNonZeroElements, numberOfFeatures, nonZeroElementIndexes, prediction, A, L, R, StoreL, StoreR, convergenceCoefficient, delta) default(none)
    {

        #pragma omp for collapse(2) private(l, k) schedule(guided)
        for (int l = 0; l < numberOfNonZeroElements; l++) {
            for (int k = 0; k < numberOfFeatures; k++) {
                #pragma omp atomic
                prediction[l] += L[nonZeroElementIndexes[l][0]][k] * R[k][nonZeroElementIndexes[l][1]];
            }
        }

        #pragma omp for private(l) schedule(guided)
        for (int l = 0; l < numberOfNonZeroElements; l++) {
            delta[l] = A[nonZeroElementIndexes[l][0]][nonZeroElementIndexes[l][1]] - prediction[l];
        }

        #pragma omp for private(l, k) collapse(2) schedule(guided)
        for (int l = 0; l < numberOfNonZeroElements; l++) {
            for (int k = 0; k < numberOfFeatures; k++) {
                #pragma omp atomic
                L[nonZeroElementIndexes[l][0]][k] += convergenceCoefficient * (2 * delta[l] * StoreR[k][nonZeroElementIndexes[l][1]]);
                #pragma omp atomic
                R[k][nonZeroElementIndexes[l][1]] += convergenceCoefficient * (2 * delta[l] * StoreL[nonZeroElementIndexes[l][0]][k]);
            }
        }
    };
};