#include "initialLR.h"

#define RAND01 ((double)random() / (double)RAND_MAX)

void initialLR(double *&L, double *&R, int &numberOfUsers, int &numberOfItems, int &numberOfFeatures) {

    srandom(1);
    for (int i = 0; i < numberOfUsers * numberOfFeatures; i++)
        L[i] = RAND01 / (double) numberOfFeatures;
    for (int k = 0; k < numberOfFeatures * numberOfItems; k++)
        R[k] = RAND01 / (double) numberOfFeatures;

};
