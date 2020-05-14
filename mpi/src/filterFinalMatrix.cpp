#include <iostream>
#include "filterFinalMatrix.h"
#include "computeB.h"

void filterFinalMatrix(std::vector<std::vector<double>> &A, std::vector<std::vector<double>> &B,
                       std::vector<int> &nonZeroUserIndexes,
                       std::vector<int> &nonZeroItemIndexes,
                       std::vector<double> &nonZeroElements,
                       std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
                       int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
                       int &numberOfNonZeroElements,
                       std::vector<int> &BV) {

        computeB(L, R, numberOfUsers, numberOfItems, numberOfFeatures, B);

        for (int l = 0; l < numberOfNonZeroElements; l++) {
            B[nonZeroUserIndexes[l]][nonZeroItemIndexes[l]] = 0;
        }

        for (int i = 0; i < numberOfUsers; i++) {
            BV[i] = std::distance(B[i].begin(), std::max_element(B[i].begin(), B[i].end()));
            std::cout << BV[i] << std::endl;
        };

//	#pragma omp for private(i) ordered
//    for (int i = 0; i < numberOfUsers; i++) {
//		#pragma omp ordered
//        std::cout << BV[i] << std::endl;
//    };

};