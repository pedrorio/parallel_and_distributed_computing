//
// Created by Pedro Rio on 14/03/2020.
//

#include "initialLR.h"

#define RAND01 ((double)random() / (double)RAND_MAX)

void initialLR(std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
               int &numberOfUsers, int &numberOfItems, int &numberOfFeatures) {

    L.resize(numberOfUsers);
    for (int i = 0; i < numberOfUsers; i++)
        L[i].resize(numberOfFeatures, 0);

    R.resize(numberOfFeatures);
    for (int i = 0; i < numberOfFeatures; i++)
        R[i].resize(numberOfItems, 0);

    srandom(1);
    for (int i = 0; i < numberOfUsers; i++)
        for (int j = 0; j < numberOfFeatures; j++)
            L[i][j] = RAND01 / (double) numberOfFeatures;
    for (int i = 0; i < numberOfFeatures; i++)
        for (int j = 0; j < numberOfItems; j++)
            R[i][j] = RAND01 / (double) numberOfFeatures;
};