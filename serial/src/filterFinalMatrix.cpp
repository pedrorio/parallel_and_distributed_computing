#include <iostream>
#include "filterFinalMatrix.h"
#include "computeB.h"
#include "printMatrix.h"

void filterFinalMatrix(double *&A, double *&B,
                       int *&nonZeroUserIndexes,
                       int *&nonZeroItemIndexes,
                       double *&nonZeroElements,
                       double *&L,
                       double *&R,
                       int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
                       int &numberOfNonZeroElements,
                       int *&BV) {

    computeB(L, R, numberOfUsers, numberOfItems, numberOfFeatures, B);

    for (int l = 0; l < numberOfNonZeroElements; l++) {
        B[nonZeroUserIndexes[l] * numberOfItems + nonZeroItemIndexes[l]] = 0;
    }

    for (int i = 0; i < numberOfUsers; i++) {
        double max = 0;
        int maxPosition;
        for (int j = 0; j < numberOfItems; j++) {
            if (B[i * numberOfItems + j] > max) {
                max = B[i * numberOfItems + j];
                maxPosition = j;
            } else {
                continue;
            }
        }
        BV[i] = maxPosition;
    }

//    printMatrix("A", A, numberOfUsers, numberOfItems);
//    printMatrix("L", L, numberOfUsers, numberOfFeatures);
//    printMatrix("R", R, numberOfFeatures, numberOfUsers);
//    printMatrix("B", B, numberOfUsers, numberOfItems);

    for (int i = 0; i < numberOfUsers; i++) {
        std::cout << BV[i] << std::endl;
    }
};