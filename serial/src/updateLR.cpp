//
// Created by Pedro Rio on 19/03/2020.
//

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

    for (int l = 0; l < numberOfNonZeroElements; l++) {
        for (int k = 0; k < numberOfFeatures; k++) {
            prediction[l] += L[nonZeroElementIndexes[l][0]][k] * R[k][nonZeroElementIndexes[l][1]];
        }
    }

    for (int l = 0; l < numberOfNonZeroElements; l++) {
        delta[l] = A[nonZeroElementIndexes[l][0]][nonZeroElementIndexes[l][1]] - prediction[l];
    }

    for (int l = 0; l < numberOfNonZeroElements; l++) {
        for (int k = 0; k < numberOfFeatures; k++) {
            L[nonZeroElementIndexes[l][0]][k] +=
                    convergenceCoefficient * (2 * delta[l] * StoreR[k][nonZeroElementIndexes[l][1]]);
            R[k][nonZeroElementIndexes[l][1]] +=
                    convergenceCoefficient * (2 * delta[l] * StoreL[nonZeroElementIndexes[l][0]][k]);
        }
    }

};