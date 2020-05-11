#ifndef SERIAL_INITIALLR_H
#define SERIAL_INITIALLR_H

#include <stdlib.h>

#define RAND01 ((double)random() / (double)RAND_MAX)

[L[][], R[][]] initialLR(int &numberOfUsers, int &numberOfItems, int &numberOfFeatures) {
    double L[numberOfUsers][numberOfFeatures];
    double R[numberOfFeatures][numberOfItems];

    srandom(1);
    for (int i = 0; i < numberOfUsers; i++)
        for (int k = 0; k < numberOfFeatures; k++)
            L[i][k] = RAND01 / (double) numberOfFeatures;
    for (int k = 0; k < numberOfFeatures; k++)
        for (int j = 0; j < numberOfItems; j++)
            R[k][j] = RAND01 / (double) numberOfFeatures;

};

#endif //SERIAL_INITIALLR_H
