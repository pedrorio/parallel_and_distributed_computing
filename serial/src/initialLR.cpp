//
// Created by Pedro Rio on 14/03/2020.
//

#include "initialLR.h"

#define RAND01 ((double)random() / (double)RAND_MAX)

void initialLR(std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
               int &numberOfUsers, int &numberOfItems, int &numberOfLatentFeatures) {

    L.resize(numberOfUsers);
    for (int i = 0; i < numberOfUsers; i++)
        L[i].resize(numberOfLatentFeatures, 0);

    R.resize(numberOfLatentFeatures);
    for (int i = 0; i < numberOfLatentFeatures; i++)
        R[i].resize(numberOfItems, 0);

    srandom(1);
    for (int i = 0; i < numberOfUsers; i++)
        for (int j = 0; j < numberOfLatentFeatures; j++)
            L[i][j] = RAND01 / (double) numberOfLatentFeatures;
    for (int i = 0; i < numberOfLatentFeatures; i++)
        for (int j = 0; j < numberOfItems; j++)
            R[i][j] = RAND01 / (double) numberOfLatentFeatures;
};