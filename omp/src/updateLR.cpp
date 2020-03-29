//#include "updateLR.h"
//
//void updateLR(std::vector<std::vector<double>> &A, std::vector<std::vector<int>> &nonZeroElementIndexes,
//              std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
//              std::vector<std::vector<double>> &StoreL, std::vector<std::vector<double>> &StoreR,
//              int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
//              int &numberOfNonZeroElements,
//              double &convergenceCoefficient) {
//
//    std::vector<std::vector<double>> prediction(nonZeroElementIndexes.size(), std::vector<double>(2));
//
//    double prediction_i_j;
//    double delta_i_j;
//    int i, j, k;
//
//    #pragma omp parallel for collapse(2) private(i,j,k, prediction_i_j, delta_i_j) schedule(static)
//    for (int i = 0; i < numberOfUsers; i++) {
//        for (int j = 0; j < numberOfItems; j++) {
//            if (A[i][j] != 0) {
//
//                prediction_i_j = 0;
//                for (int k = 0; k < numberOfFeatures; k++) {
////#pragma omp atomic
//                    prediction_i_j = prediction_i_j + L[i][k] * R[k][j];
//                }
//                delta_i_j = A[i][j] - prediction_i_j;
//
//                for (int k = 0; k < numberOfFeatures; k++) {
////#pragma omp atomic
//                    L[i][k] = L[i][k] + convergenceCoefficient * (2 * delta_i_j * StoreR[k][j]);
////#pragma omp atomic
//                    R[k][j] = R[k][j] + convergenceCoefficient * (2 * delta_i_j * StoreL[i][k]);
//                }
//            }
//        }
//    }
//};



//
//#include "updateLR.h"
//
//void updateLR(std::vector<std::vector<double>> &A, std::vector<std::vector<int>> &nonZeroElementIndexes,
//              std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
//              std::vector<std::vector<double>> &StoreL, std::vector<std::vector<double>> &StoreR,
//              int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
//              int &numberOfNonZeroElements,
//              double &convergenceCoefficient) {
//
//    std::vector<double> prediction(nonZeroElementIndexes.size(), 0);
//    std::vector<double> delta(nonZeroElementIndexes.size(), 0);
//
//    int l, k;
//
////#pragma omp parallel for private(l, k) schedule(static)
//    for (int l = 0; l < numberOfNonZeroElements; l++) {
//        for (int k = 0; k < numberOfFeatures; k++) {
//#pragma omp atomic
//            prediction[l] += L[nonZeroElementIndexes[l][0]][k] * R[k][nonZeroElementIndexes[l][1]];
//        }
//    }
//
////#pragma omp parallel for private(l) schedule(static)
//    for (int l = 0; l < numberOfNonZeroElements; l++) {
//        delta[l] = A[nonZeroElementIndexes[l][0]][nonZeroElementIndexes[l][1]] - prediction[l];
//    }
//
//
////#pragma omp parallel for private(l, k) collapse(2) schedule(static)
//    for (int l = 0; l < numberOfNonZeroElements; l++) {
//        for (int k = 0; k < numberOfFeatures; k++) {
//#pragma omp atomic
//            L[nonZeroElementIndexes[l][0]][k] +=
//                    convergenceCoefficient * (2 * delta[l] * StoreR[k][nonZeroElementIndexes[l][1]]);
//#pragma omp atomic
//            R[k][nonZeroElementIndexes[l][1]] +=
//                    convergenceCoefficient * (2 * delta[l] * StoreL[nonZeroElementIndexes[l][0]][k]);
//        }
//    }
//};


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