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

    double prediction_i_j;

#pragma omp parallel for collapse(2) private(i,j)
    for (int i = 0; i < numberOfUsers; i++) {
        for (int j = 0; j < numberOfItems; j++) {
//#pragma omp atomic
            if (A[i][j] > 0) {

                prediction_i_j = 0;

                for (int k = 0; k < numberOfFeatures; k++) {
                    prediction_i_j += L[i][k] * R[k][j];
                }
                double delta_i_j = A[i][j] - prediction_i_j;

                for (int k = 0; k < numberOfFeatures; k++) {
                    L[i][k] = L[i][k] + convergenceCoefficient * (2 * delta_i_j * StoreR[k][j]);
                    R[k][j] = R[k][j] + convergenceCoefficient * (2 * delta_i_j * StoreL[i][k]);
                }
            }
        }
    }

};