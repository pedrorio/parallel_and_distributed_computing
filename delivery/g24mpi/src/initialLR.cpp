#include "initialLR.h"

#define RAND01 ((double)random() / (double)RAND_MAX)

void initialLR(double *&L, double *&R,
               int &numberOfUsers, int &numberOfItems, int &numberOfFeatures) {
    srandom(1);
    for (int i = 0; i < numberOfUsers; i++)
        for (int k = 0; k < numberOfFeatures; k++)
            L[i * numberOfFeatures + k] = RAND01 / (double) numberOfFeatures;
    for (int k = 0; k < numberOfFeatures; k++)
        for (int j = 0; j < numberOfItems; j++)
            R[k * numberOfItems + j] = RAND01 / (double) numberOfFeatures;
}
