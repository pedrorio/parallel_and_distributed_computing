#include "initialLR.h"

#define RAND01 ((double)random() / (double)RAND_MAX)

void initialLR(std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
               int &numberOfUsers, int &numberOfItems, int &numberOfFeatures) {

    srandom(1);
    for (int i = 0; i < numberOfUsers; i++)
        for (int j = 0; j < numberOfFeatures; j++)
            L[i][j] = RAND01 / (double) numberOfFeatures;
    for (int i = 0; i < numberOfFeatures; i++)
        for (int j = 0; j < numberOfItems; j++)
            R[i][j] = RAND01 / (double) numberOfFeatures;
};