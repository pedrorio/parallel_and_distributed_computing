#include <iostream>
#include "filterFinalMatrix.h"
#include "computeB.h"

void filterFinalMatrix(std::vector<std::vector<double>> &A, std::vector<std::vector<double>> &B,
                       std::vector<std::vector<int>> &nonZeroElementIndexes,
                       std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
                       int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
                       int &numberOfNonZeroElements,
                       std::vector<int> &BV) {

    int i, j, l;
	#pragma omp parallel shared(numberOfUsers, numberOfItems, numberOfFeatures, A, B, L, R, numberOfNonZeroElements, nonZeroElementIndexes, BV, std::cout)  default(none) private(i,l,j)
    {
        computeB(L, R, numberOfUsers, numberOfItems, numberOfFeatures, B);

		#pragma omp for private(l) schedule(guided)
        for (int l = 0; l < numberOfNonZeroElements; l++) {
            B[nonZeroElementIndexes[l][0]][nonZeroElementIndexes[l][1]] = 0;
        }

		#pragma omp for private(i) schedule(guided) ordered
        for (int i = 0; i < numberOfUsers; i++) {
            BV[i] = std::distance(B[i].begin(), std::max_element(B[i].begin(), B[i].end()));
            #pragma omp ordered
            std::cout << BV[i] << std::endl;
        };
    };

//	#pragma omp for private(i) ordered
//    for (int i = 0; i < numberOfUsers; i++) {
//		#pragma omp ordered
//        std::cout << BV[i] << std::endl;
//    };

};