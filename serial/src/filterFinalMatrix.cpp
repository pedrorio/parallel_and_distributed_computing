//
// Created by Pedro Rio on 26/03/2020.
//

#include <iostream>
#include "filterFinalMatrix.h"
#include "computeB.h"

void filterFinalMatrix(std::vector<std::vector<double>> &A, std::vector<std::vector<int>> &nonZeroElementIndexes,
                       std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
                       int &numberOfUsers, int &numberOfItems, int &numberOfFeatures) {

    std::vector<std::vector<double>> B(numberOfUsers, std::vector<double>(numberOfItems));
    std::vector<int> BV;

    B = computeB(L, R,
             numberOfUsers, numberOfItems, numberOfFeatures);

    for (int i = 0; i < numberOfUsers; i++) {
        for (int j = 0; j < numberOfItems; j++) {
            if (A[i][j] != 0) {
                B[i][j] = 0;
            }
        }
        int maxElement = std::distance(B[i].begin(), std::max_element(B[i].begin(), B[i].end()));
        BV.push_back(maxElement);
        std::cout << maxElement << std::endl;
    };

//    for (int i = 0; i < numberOfUsers; i++) {
//        for (int j = 0; j < numberOfItems; j++) {
//            if (A[i][j] != 0) {
//                B[i][j] = 0;
//                continue;
//            }
//
//            for (int k = 0; k < numberOfFeatures; k++) {
//                B[i][j] += L[i][k] * R[k][j];
//            }
//        }
//        int maxElement = std::distance(B[i].begin(), std::max_element(B[i].begin(), B[i].end()));
//        BV.push_back(maxElement);
//        std::cout << maxElement << std::endl;
//    };
};
