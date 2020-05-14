#include "updateLR.h"

void updateLR(std::vector<std::vector<double>> &A,
              std::vector<int> &nonZeroUserIndexes,
              std::vector<int> &nonZeroItemIndexes,
              std::vector<double> &nonZeroElements,
              std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
              std::vector<std::vector<double>> &StoreL, std::vector<std::vector<double>> &StoreR,
              int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
              int &numberOfNonZeroElements,
              double &convergenceCoefficient) {

    std::vector<double> prediction(numberOfNonZeroElements, 0);
    std::vector<double> delta(numberOfNonZeroElements, 0);

    for (int l = 0; l < numberOfNonZeroElements; l++) {
        for (int k = 0; k < numberOfFeatures; k++) {
            prediction[l] += L[nonZeroUserIndexes[l]][k] * R[k][nonZeroItemIndexes[l]];
        }
    }

    for (int l = 0; l < numberOfNonZeroElements; l++) {
        delta[l] = A[nonZeroUserIndexes[l]][nonZeroItemIndexes[l]] - prediction[l];
    }

    for (int l = 0; l < numberOfNonZeroElements; l++) {
        for (int k = 0; k < numberOfFeatures; k++) {
            L[nonZeroUserIndexes[l]][k] +=
                    convergenceCoefficient * (2 * delta[l] * StoreR[k][nonZeroItemIndexes[l]]);
            R[k][nonZeroItemIndexes[l]] +=
                    convergenceCoefficient * (2 * delta[l] * StoreL[nonZeroUserIndexes[l]][k]);
        }
    }
}

