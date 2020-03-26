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

//    for (int l = 0; l < numberOfNonZeroElements; l++) {
//        int i = nonZeroElementIndexes[l][0];
//        int j = nonZeroElementIndexes[l][1];
//
//        if (A[i][j] > 0) {
//
//            // dot product
//            prediction_i_j = 0;
//            for (int k = 0; k < numberOfFeatures; k++) {
//                prediction_i_j += L[i][k] * R[k][j];
//            }
//            double delta_i_j = A[i][j] - prediction_i_j;
//
//            for (int k = 0; k < numberOfFeatures; k++) {
//                L[i][k] = L[i][k] + convergenceCoefficient * (2 * delta_i_j * StoreR[k][j]);
//                R[k][j] = R[k][j] + convergenceCoefficient * (2 * delta_i_j * StoreL[i][k]);
//            }
//        }
//
//    }

    for (int i = 0; i < A.size(); i++) {
        for (int j = 0; j < A[i].size(); j++) {
            if (A[i][j] > 0) {

                // dot product
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