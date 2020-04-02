#include <iostream>
#include "filterFinalMatrix.h"
#include "computeB.h"

void filterFinalMatrix(std::vector<std::vector<double>> &A, std::vector<std::vector<double>> &B, std::vector<std::vector<int>> &nonZeroElementIndexes,
                       std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
                       int &numberOfUsers, int &numberOfItems, int &numberOfFeatures, int &numberOfNonZeroElements,
                       std::vector<int> &BV) {

    int i, j, l;
    std::vector<double> BMax(numberOfUsers, 0);
	#pragma omp parallel shared(numberOfUsers, numberOfItems, numberOfFeatures, A, B, L, R, numberOfNonZeroElements, nonZeroElementIndexes, BV, BMax)  default(none) private(i,l,j)
    {
        computeB(L, R, numberOfUsers, numberOfItems, numberOfFeatures, B);

		#pragma omp for private(l) schedule(guided)
        for (int l = 0; l < numberOfNonZeroElements; l++) {
            B[nonZeroElementIndexes[l][0]][nonZeroElementIndexes[l][1]] = 0;
        }

		#pragma omp for private(i) schedule(guided)
        for (int i = 0; i < numberOfUsers; i++) {
            int maxElementIndex = std::distance(B[i].begin(), std::max_element(B[i].begin(), B[i].end()));
            BV[i] = maxElementIndex;
        };

//#pragma omp for private(i,j) collapse(2) schedule(static)
//        for (int i = 0; i < numberOfUsers; i++) {
//            for (int j=0; j<numberOfItems; j++) {
////                #pragma omp critical
////                {
//                    if (B[i][j] > BMax[i]) {
//#pragma omp atomic write
//                        BMax[i] = B[i][j];
//#pragma omp atomic write
//                        BV[i] = j;
//                    }
//                }
////            }
////            BV[i] = std::distance(B[i].begin(), std::max_element(B[i].begin(), B[i].end()));
//        };



    };

	#pragma omp for private(i) ordered
    for (int i = 0; i < numberOfUsers; i++) {
		#pragma omp ordered
        std::cout << BV[i] << std::endl;
    };

};